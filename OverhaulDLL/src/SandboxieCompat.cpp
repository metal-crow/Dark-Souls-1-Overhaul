/*
    DARK SOULS OVERHAUL

    Sandboxie + Seamless Co-op entry point compatibility.

    Sandboxie's SbieDll.dll is loaded before any of the game's static imports, and it hooks the PE entry
    point of every sandboxed exe with a 12 byte absolute jump (core/dll/ldr_init.c, Ldr_Inject_Init):
            mov rax, SbieDll!Ldr_Inject_Entry64 ; jmp rax
    When the loader reaches the entry point that hook runs Ldr_Inject_Entry, which puts the original bytes
    back, does Sandboxie's entry-time work (InjectDll= DLLs, Dll_InitExeEntry) and jumps to the real entry.

    Seamless Co-op embeds tremwil's arxan-disabler. Before the entry point runs it assumes the entry point
    is Arxan's wrapper
            sub rsp,28h ; call <arxan init stub> ; add rsp,28h ; jmp <real entry>
    and detours the call by rewriting the rel32 at entry+5, without checking what is there. Under Sandboxie
    those four bytes are the middle of Sandboxie's imm64, so the jmp rax now goes to a non-canonical address
    and the first thing to call the entry point (SC's launcher thread) dies with an access violation.

    We run from DllMain, after SbieDll and before ds1sc.dll, and make the two agree:
      1. put the original entry bytes back, so arxan-disabler finds the wrapper it expects,
      2. but aim the wrapper's call at our own detour (via a code cave in rel32 range),
      3. when that detour eventually runs - straight from the loader, or through arxan-disabler's detour
         chained on top of it - do Sandboxie's Ldr_Inject_Entry first, then the game's real Arxan init stub,
         which is exactly what the original call would have done.
    Not sandboxed, or a Sandboxie hook we don't recognise: leave everything alone.
*/

#include "SandboxieCompat.h"
#include "DarkSoulsOverhaulMod.h"
#include <windows.h>
#include <cstdint>
#include <cstring>

namespace {
    // Length of Sandboxie's mov rax,imm64 ; jmp rax, and the number of original bytes it saves/restores
    const size_t ENTRY_HOOK_LEN = 12;

    uint8_t* entry_point = NULL;
    uint8_t  original_entry[ENTRY_HOOK_LEN];
    void (*real_arxan_init)() = NULL;

    // SbieDll!Ldr_Inject_Entry: restores the entry bytes, does Sandboxie's entry-time init, returns the entry point
    typedef void* (*Ldr_Inject_Entry_t)(uintptr_t* pPtr);
    Ldr_Inject_Entry_t sbie_Ldr_Inject_Entry = NULL;
}

// Read the entry point's bytes as they are in the exe on disk, i.e. before Sandboxie touched them
static bool read_original_entry_bytes(uint8_t* out, size_t len)
{
    char path[MAX_PATH];
    if (!GetModuleFileNameA(NULL, path, sizeof(path)))
        return false;

    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (f == INVALID_HANDLE_VALUE)
        return false;

    bool ok = false;
    uint8_t hdr[0x1000];
    DWORD n = 0;
    if (ReadFile(f, hdr, sizeof(hdr), &n, NULL) && n == sizeof(hdr))
    {
        IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)hdr;
        if (dos->e_magic == IMAGE_DOS_SIGNATURE && dos->e_lfanew > 0 && dos->e_lfanew < 0x800)
        {
            IMAGE_NT_HEADERS64* nt = (IMAGE_NT_HEADERS64*)(hdr + dos->e_lfanew);
            uint32_t ep_rva = nt->OptionalHeader.AddressOfEntryPoint;
            IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
            for (int i = 0; i < nt->FileHeader.NumberOfSections; i++, sec++)
            {
                if (ep_rva >= sec->VirtualAddress && ep_rva + len <= sec->VirtualAddress + sec->SizeOfRawData)
                {
                    LONG off = (LONG)(sec->PointerToRawData + (ep_rva - sec->VirtualAddress));
                    ok = SetFilePointer(f, off, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER
                        && ReadFile(f, out, (DWORD)len, &n, NULL) && n == len;
                    break;
                }
            }
        }
    }
    CloseHandle(f);
    return ok;
}

// Executable memory close enough to `anchor` for a rel32 to reach it
static uint8_t* alloc_near(void* anchor, size_t size)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    uintptr_t gran = si.dwAllocationGranularity;
    uintptr_t base = (uintptr_t)anchor & ~(gran - 1);
    for (uintptr_t d = gran; d < 0x70000000; d += gran)
    {
        void* p = VirtualAlloc((void*)(base - d), size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (p) return (uint8_t*)p;
        p = VirtualAlloc((void*)(base + d), size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (p) return (uint8_t*)p;
    }
    return NULL;
}

// Ldr_Inject_Entry64 (core/dll/util_64.asm) is
//     sub rsp,28h ; call Ldr_Inject_Entry ; mov rdx,rax ; add rsp,28h ; <zero 200h bytes below rsp> ; jmp rdx
// Decode the call to get Ldr_Inject_Entry, the C function that returns instead of jumping.
static Ldr_Inject_Entry_t find_Ldr_Inject_Entry(uint8_t* Ldr_Inject_Entry64)
{
    HMODULE sbie = GetModuleHandleA("SbieDll.dll");
    if (!sbie)
        return NULL;
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)((uint8_t*)sbie + ((IMAGE_DOS_HEADER*)sbie)->e_lfanew);
    uint8_t* lo = (uint8_t*)sbie;
    uint8_t* hi = lo + nt->OptionalHeader.SizeOfImage;

    const int scan = 48;
    if (Ldr_Inject_Entry64 < lo || Ldr_Inject_Entry64 + scan > hi)
        return NULL;

    static const uint8_t prologue[] = { 0x48, 0x83, 0xEC, 0x28, 0xE8 }; // sub rsp,28h ; call
    if (memcmp(Ldr_Inject_Entry64, prologue, sizeof(prologue)) != 0)
        return NULL;

    bool jmp_rdx = false;
    for (int i = 9; i < scan - 1; i++)
    {
        if (Ldr_Inject_Entry64[i] == 0xFF && Ldr_Inject_Entry64[i + 1] == 0xE2)
        {
            jmp_rdx = true;
            break;
        }
    }
    if (!jmp_rdx)
        return NULL;

    uint8_t* target = Ldr_Inject_Entry64 + 9 + *(int32_t*)(Ldr_Inject_Entry64 + 5);
    if (target < lo || target >= hi)
        return NULL;
    return (Ldr_Inject_Entry_t)target;
}

// Stands in for the game's Arxan init stub. Reached from the entry wrapper's call, either directly from the
// loader or through arxan-disabler's detour, which unhooks itself and then calls what it found there: us.
static void sandboxie_entry_detour()
{
    if (sbie_Ldr_Inject_Entry)
    {
        // Sandboxie's entry-point work: restores the original entry bytes, loads InjectDll= DLLs, Dll_InitExeEntry
        uintptr_t ep = (uintptr_t)entry_point;
        sbie_Ldr_Inject_Entry(&ep);
    }
    real_arxan_init();
}

void SandboxieCompat::start()
{
    if (GetModuleHandleA("SbieDll.dll") == NULL)
        return;

    ConsoleWrite("Sandboxie detected, reconciling its entry point hook with Seamless Co-op...");

    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)GetModuleHandleA(NULL);
    IMAGE_NT_HEADERS64* nt = (IMAGE_NT_HEADERS64*)((uint8_t*)dos + dos->e_lfanew);
    entry_point = (uint8_t*)dos + nt->OptionalHeader.AddressOfEntryPoint;

    // Sandboxie's hook: 48 B8 <imm64> FF E0
    if (!(entry_point[0] == 0x48 && entry_point[1] == 0xB8 && entry_point[10] == 0xFF && entry_point[11] == 0xE0))
    {
        ConsoleWrite("    entry point doesn't carry Sandboxie's hook (%02X %02X .. %02X %02X), leaving it alone",
            entry_point[0], entry_point[1], entry_point[10], entry_point[11]);
        return;
    }
    uint8_t* Ldr_Inject_Entry64 = *(uint8_t**)(entry_point + 2);

    if (!read_original_entry_bytes(original_entry, sizeof(original_entry)))
    {
        ConsoleWrite("    unable to read the entry point from the exe on disk, leaving it alone");
        return;
    }
    // Arxan's entry wrapper: sub rsp,28h ; call rel32 ; add rsp,28h
    static const uint8_t sub_rsp_call[] = { 0x48, 0x83, 0xEC, 0x28, 0xE8 };
    static const uint8_t add_rsp[] = { 0x48, 0x83, 0xC4 };
    if (memcmp(original_entry, sub_rsp_call, sizeof(sub_rsp_call)) != 0 || memcmp(original_entry + 9, add_rsp, sizeof(add_rsp)) != 0)
    {
        ConsoleWrite("    entry point on disk isn't the Arxan wrapper we expect, leaving it alone");
        return;
    }
    real_arxan_init = (void(*)())(entry_point + 9 + *(int32_t*)(original_entry + 5));

    sbie_Ldr_Inject_Entry = find_Ldr_Inject_Entry(Ldr_Inject_Entry64);
    if (!sbie_Ldr_Inject_Entry)
        ConsoleWrite("    WARNING: couldn't resolve SbieDll!Ldr_Inject_Entry from %p, Sandboxie's entry-point init will be skipped", Ldr_Inject_Entry64);

    // Code cave the wrapper's rel32 call can reach: mov rax, sandboxie_entry_detour ; jmp rax
    uint8_t* cave = alloc_near(entry_point, ENTRY_HOOK_LEN);
    if (!cave)
    {
        ConsoleWrite("    no executable memory within rel32 range of the entry point, leaving it alone");
        return;
    }
    cave[0] = 0x48;
    cave[1] = 0xB8;
    *(void**)(cave + 2) = (void*)&sandboxie_entry_detour;
    cave[10] = 0xFF;
    cave[11] = 0xE0;

    uint8_t patched[ENTRY_HOOK_LEN];
    memcpy(patched, original_entry, sizeof(patched));
    *(int32_t*)(patched + 5) = (int32_t)(cave - (entry_point + 9));

    DWORD old_protect;
    if (!VirtualProtect(entry_point, ENTRY_HOOK_LEN, PAGE_EXECUTE_READWRITE, &old_protect))
    {
        ConsoleWrite("    VirtualProtect on the entry point failed (%u), leaving it alone", GetLastError());
        return;
    }
    memcpy(entry_point, patched, ENTRY_HOOK_LEN);
    VirtualProtect(entry_point, ENTRY_HOOK_LEN, old_protect, &old_protect);
    FlushInstructionCache(GetCurrentProcess(), entry_point, ENTRY_HOOK_LEN);

    ConsoleWrite("    entry point restored; Sandboxie's Ldr_Inject_Entry (%p) and the Arxan init stub (%p) now run from our detour",
        sbie_Ldr_Inject_Entry, real_arxan_init);
}
