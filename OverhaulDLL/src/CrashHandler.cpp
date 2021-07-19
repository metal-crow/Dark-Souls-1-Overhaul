/*
 * This specific file is released under the MIT License
 * Copyright(c) 2020 Metal-Crow.
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "CrashHandler.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include "StackWalker.h"
#include "crash_hander_ui_resource.h"
#include "quickstart.h"

#include <eh.h>
#include <new.h>
#include <signal.h>
#include <Windows.h>
#include <direct.h>
#include <ctime>
#include <string>

uint64_t panic_debug_offset = 0xd4ff86;
extern "C" {
    uint64_t panic_debug_return;
    void panic_debug_injection();
    void panic_debug_injection_helper_function();
}

/* ------------------ UI ------------------ */

void get_ui_text(HWND hwnd, int object, char** output) {
    HWND obj = GetDlgItem(hwnd, object);
    int text_len = GetWindowTextLength(obj);
    if (text_len > 0) {
        int text_buffer_len = text_len + 1;
        *output = (char*)malloc(text_buffer_len);
        GetWindowText(obj, *output, text_buffer_len);
    }
    CloseHandle(obj);
}

char* error_description_text = nullptr;
char* user_name = nullptr;
char* user_email = nullptr;

LRESULT CALLBACK crash_handler_ui_proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                    // Save the crash info given
                    get_ui_text(hwnd, IDC_DESC, &error_description_text);
                    get_ui_text(hwnd, IDC_NAME, &user_name);
                    get_ui_text(hwnd, IDC_EMAIL, &user_email);

                    EndDialog(hwnd, 1);
                    break;
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    break;
            }
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

INT_PTR WINAPI make_crash_handler_ui()
{
    return DialogBox(d3d11_module, MAKEINTRESOURCE(IDD_DIALOG1), NULL, crash_handler_ui_proc);
}

/* ------------------ Helpers ------------------ */

std::string exception_record_tostring(struct _EXCEPTION_RECORD *ExceptionRecord)
{
    std::string out;
    char scratch[128];

    snprintf(scratch, sizeof(scratch), "-------------------\nExceptionRecord:\n");
    out.append(scratch);

    snprintf(scratch, sizeof(scratch), "ExceptionCode=%u\nExceptionFlags=%u\n", ExceptionRecord->ExceptionCode, ExceptionRecord->ExceptionFlags);
    out.append(scratch);

    if (ExceptionRecord->ExceptionRecord != NULL) {
        std::string recursed_str = exception_record_tostring(ExceptionRecord->ExceptionRecord);
        out.append(recursed_str);
    }

    snprintf(scratch, sizeof(scratch), "ExceptionAddress=%llx\nNumberParameters=%u\n", (uint64_t)ExceptionRecord->ExceptionAddress, ExceptionRecord->NumberParameters);
    out.append(scratch);

    for (size_t i = 0; i < ExceptionRecord->NumberParameters; i++) {
        snprintf(scratch, sizeof(scratch), "ExceptionRecord[%zd]=%llx\n", i, ExceptionRecord->ExceptionInformation[i]);
        out.append(scratch);
    }

    snprintf(scratch, sizeof(scratch), "-------------------\n");
    out.append(scratch);

    return out;
}

std::string context_record_tostring(PCONTEXT ContextRecord)
{
    std::string out;
    char scratch[128];

    snprintf(scratch, sizeof(scratch), "-------------------\nContextRecord:\n");
    out.append(scratch);

    snprintf(scratch, sizeof(scratch), "ContextFlags=%x", ContextRecord->ContextFlags);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "MxCsr=%x", ContextRecord->MxCsr);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "SegCs=%x", ContextRecord->SegCs);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "SegDs=%x", ContextRecord->SegDs);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "SegEs=%x", ContextRecord->SegEs);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "SegFs=%x", ContextRecord->SegFs);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "SegGs=%x", ContextRecord->SegGs);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "SegSs=%x", ContextRecord->SegSs);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "EFlags=%x", ContextRecord->EFlags);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr0=%llx", ContextRecord->Dr0);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr1=%llx", ContextRecord->Dr1);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr2=%llx", ContextRecord->Dr2);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr3=%llx", ContextRecord->Dr3);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr6=%llx", ContextRecord->Dr6);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr7=%llx", ContextRecord->Dr7);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rax=%llx", ContextRecord->Rax);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rcx=%llx", ContextRecord->Rcx);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rdx=%llx", ContextRecord->Rdx);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rbx=%llx", ContextRecord->Rbx);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rsp=%llx", ContextRecord->Rsp);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rbp=%llx", ContextRecord->Rbp);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rsi=%llx", ContextRecord->Rsi);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rdi=%llx", ContextRecord->Rdi);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "R8=%llx", ContextRecord->R8);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "R9=%llx", ContextRecord->R9);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "R10=%llx", ContextRecord->R10);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "R11=%llx", ContextRecord->R11);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "R12=%llx", ContextRecord->R12);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "R13=%llx", ContextRecord->R13);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "R14=%llx", ContextRecord->R14);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "R15=%llx", ContextRecord->R15);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rip=%llx", ContextRecord->Rip);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm0=%llx %llx", (ContextRecord->Xmm0.High), (ContextRecord->Xmm0.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm1=%llx %llx", (ContextRecord->Xmm1.High), (ContextRecord->Xmm1.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm2=%llx %llx", (ContextRecord->Xmm2.High), (ContextRecord->Xmm2.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm3=%llx %llx", (ContextRecord->Xmm3.High), (ContextRecord->Xmm3.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm4=%llx %llx", (ContextRecord->Xmm4.High), (ContextRecord->Xmm4.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm5=%llx %llx", (ContextRecord->Xmm5.High), (ContextRecord->Xmm5.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm6=%llx %llx", (ContextRecord->Xmm6.High), (ContextRecord->Xmm6.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm7=%llx %llx", (ContextRecord->Xmm7.High), (ContextRecord->Xmm7.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm8=%llx %llx", (ContextRecord->Xmm8.High), (ContextRecord->Xmm8.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm9=%llx %llx", (ContextRecord->Xmm9.High), (ContextRecord->Xmm9.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm10=%llx %llx", (ContextRecord->Xmm10.High), (ContextRecord->Xmm10.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm11=%llx %llx", (ContextRecord->Xmm11.High), (ContextRecord->Xmm11.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm12=%llx %llx", (ContextRecord->Xmm12.High), (ContextRecord->Xmm12.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm13=%llx %llx", (ContextRecord->Xmm13.High), (ContextRecord->Xmm13.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm14=%llx %llx", (ContextRecord->Xmm14.High), (ContextRecord->Xmm14.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Xmm15=%llx %llx", (ContextRecord->Xmm15.High), (ContextRecord->Xmm15.Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "VectorRegister[26]=%llx %llx", (ContextRecord->VectorRegister[26].High), (ContextRecord->VectorRegister[26].Low));
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "VectorControl=%llx", ContextRecord->VectorControl);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "DebugControl=%llx", ContextRecord->DebugControl);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "LastBranchToRip=%llx", ContextRecord->LastBranchToRip);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "LastBranchFromRip=%llx", ContextRecord->LastBranchFromRip);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "LastExceptionToRip=%llx", ContextRecord->LastExceptionToRip);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "LastExceptionFromRip=%llx", ContextRecord->LastExceptionFromRip);
    out.append(scratch);

    snprintf(scratch, sizeof(scratch), "-------------------\n");
    out.append(scratch);

    return out;
}

std::string* exception_pointer_tostring(EXCEPTION_POINTERS * ExceptionInfo)
{
    // We need to grab the underlying c_str and pass it to the crash handler, so make sure it can't get destructed
    // Memory leak, but we're gonna abort in a second anyway
    std::string* out = new std::string();

    std::string record_str = exception_record_tostring(ExceptionInfo->ExceptionRecord);
    out->append(record_str);

    std::string context_str = context_record_tostring(ExceptionInfo->ContextRecord);
    out->append(record_str);

    return out;
}

class MyStackWalker : public StackWalker
{
public:
    MyStackWalker() : StackWalker() {}
    std::string GetOutput() {
        //clean the buffer between callstack requests
        std::string output(output_text);
        output_text.clear();
        return output;
    }
protected:
    std::string output_text;
    virtual void OnOutput(LPCSTR szText) {
        //this is called multiple times from ShowCallstack, so need it to save and append
        output_text.append(szText);
    }

};

DWORD WINAPI crash_handler_dump_process(LPVOID output_dir)
{
    //command to pass to process dump
    char cmd[MAX_PATH + 100 + 50];
    snprintf(cmd, sizeof(cmd), "./pd64.exe -p DarkSoulsRemastered.exe -o \"%s\"", (char*)output_dir);

    // Dump the process using Process Dump (https://github.com/glmcdona/Process-Dump)
    PROCESS_INFORMATION procdump_pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&procdump_pi, sizeof(procdump_pi));
    bool procdump = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &procdump_pi);

    // Wait 15 sec max for the process dump to finish, or kill it if it doesn't
    WaitForSingleObject(procdump_pi.hProcess, 15 * 1000);
    TerminateProcess(procdump_pi.hProcess, 0);
    CloseHandle(procdump_pi.hProcess);
    CloseHandle(procdump_pi.hThread);

    //remove the extra mostly-useless modules. only need the exe and the d3d11 dumps
    std::string pattern((char*)output_dir);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string filename(data.cFileName);
            if (filename.find("DarkSoulsRemastered.exe") == std::string::npos &&
                filename.find("d3d11.dll") == std::string::npos &&
                filename.find("message") == std::string::npos &&
                filename.find(logfilename) == std::string::npos)
            {
                std::string full_filename((char*)output_dir);
                full_filename.append("\\");
                full_filename.append(data.cFileName);
                DeleteFile(full_filename.c_str());
            }
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }

    return 0;
}

/* ------------------ Handlers ------------------ */

std::string const bucket_name = "dark_souls_overhaul_bug_reports";
std::string const key_path = "./error_reporter_creds.json";
bool triggered_crash_handler = false;

void crash_handler(char* message_str)
{
    // Prevent an internal failure -> abort from infinitely recursing, and also catch any unexpected double-faults
    signal(SIGABRT, NULL);
    if (triggered_crash_handler) {
        abort();
    }
    triggered_crash_handler = true;

    // Create the locations to output the dump to
    char output_dir[MAX_PATH+100];
    GetCurrentDirectory(MAX_PATH, output_dir);
    //parent crash directory, if it doesn't exist
    strncat_s(output_dir, "\\crash", sizeof(output_dir));
    _mkdir(output_dir);
    //crash specific directory
    time_t rawtime;
    struct tm timeinfo;
    char time_str[80];
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);
    strftime(time_str, sizeof(time_str), "\\%Y-%m-%d %H-%M-%S", &timeinfo);
    strncat_s(output_dir, time_str, sizeof(output_dir));
    _mkdir(output_dir);

    // Start process dump
    HANDLE dump_thread = CreateThread(NULL, 0, crash_handler_dump_process, output_dir, 0, NULL);

    // Save the stack trace
    MyStackWalker sw;
    sw.ShowCallstack(); //This interally calls OnOutput, which we overwrite to only save the output
    std::string stack_info = sw.GetOutput(); // Get the saved output

    // Write out the stack trace
    FILE* fp;
    char msg_file[sizeof(output_dir)];
    snprintf(msg_file, sizeof(msg_file), "%s\\%s", output_dir, "message");
    fopen_s(&fp, msg_file, "w");
    fprintf(fp, "%s\n\n", stack_info.c_str());
    if (message_str != NULL) {
        fprintf(fp, "%s", message_str);
    }
    fclose(fp);

    // Copy the log file into the crash folder
    fflush(logfile);
    fclose(logfile);

    char log_file[sizeof(output_dir)];
    snprintf(log_file, sizeof(log_file), "%s\\%s", output_dir, logfilename);
    CopyFile(logfilename, log_file, false);

    // Run the UI. This won't return until the user closes the window
    INT_PTR send_report = make_crash_handler_ui();

    // Write out any message info
    fopen_s(&fp, msg_file, "a");
    if (error_description_text != nullptr) {
        fprintf(fp, "--------------------\nUser Error Report: %s\n", error_description_text);
    }
    if (user_name != nullptr) {
        fprintf(fp, "--------------------\nUser Name: %s\n", user_name);
    }
    if (user_email != nullptr) {
        fprintf(fp, "--------------------\nUser Email: %s\n", user_email);
    }
    fclose(fp);

    // Wait for the process dump to finish
    WaitForSingleObject(dump_thread, INFINITE);

    //zip the folder
    if (send_report != 0)
    {
        char cmd[MAX_PATH + 100 + 50];
        snprintf(cmd, sizeof(cmd), "powershell.exe -command \"Compress-Archive '%s' '%s.zip'\"", (char*)output_dir, (char*)output_dir);
        PROCESS_INFORMATION zip_pi;
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&zip_pi, sizeof(zip_pi));
        bool zipdump = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &zip_pi);
        // Wait 15 sec max for the zip to finish, or kill it if it doesn't
        WaitForSingleObject(zip_pi.hProcess, 15 * 1000);
        TerminateProcess(zip_pi.hProcess, 0);
        CloseHandle(zip_pi.hProcess);
        CloseHandle(zip_pi.hThread);
    }

    // Send the report
    if (send_report != 0) {
        //generate name for this upload (hostname + datetime)
        char* username;
        errno_t err = _dupenv_s(&username, NULL, "USERNAME");
        char crashname[120];
        snprintf(crashname, sizeof(crashname), "%s%s", username, time_str);

        //upload crash
        GoogleCloudLib::CreateClient(key_path);
        char zipfile[sizeof(output_dir)+4];
        snprintf(zipfile, sizeof(zipfile), "%s.zip", output_dir);
        GoogleCloudLib::UploadFile(bucket_name, zipfile, crashname);
    }
    
    // Don't give the other threads a chance to hang; kill everything immediatly
    abort();
}

void panic_debug_injection_helper_function()
{
    crash_handler();
}

void terminator()
{
    crash_handler();
}

void signal_handler(int sig)
{
    crash_handler();
}

void __cdecl invalid_parameter_handler(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t)
{
    crash_handler();
}

int memory_depleted(size_t)
{
    crash_handler();
    return 0;
}

LONG WINAPI top_level_exception_filter(EXCEPTION_POINTERS * ExceptionInfo)
{
    crash_handler((char*)exception_pointer_tostring(ExceptionInfo)->c_str());
    return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI vectored_exception_handler(EXCEPTION_POINTERS * ExceptionInfo)
{
    /* Notes:
     * The game, on startup, raises and handles some custom exceptions as part of the boot process.
     * These can't be caught since they're required to be caught by the game
     * -0x406d1388
     * -0xe06d7363
     */
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x406d1388 || ExceptionInfo->ExceptionRecord->ExceptionCode == 0xe06d7363) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    /* Notes:
     * I have also encountered other standard windows exceptions being raised for unclear reasons.
     * https://stackoverflow.com/questions/12298406/how-to-treat-0x40010006-exception-in-vectored-exception-handler specifies that some of these are expected to be ignored.
     * To error on the side of safety, only catch exceptions starting with 0xC0
     */
    if ((ExceptionInfo->ExceptionRecord->ExceptionCode & 0xFF000000) != 0xC0000000) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    crash_handler((char*)exception_pointer_tostring(ExceptionInfo)->c_str());
    return EXCEPTION_CONTINUE_SEARCH;
}

/* ------------------ Main Function ------------------ */

void set_crash_handlers()
{
    ConsoleWrite("Adding crash handlers...");

    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-terminate-crt?view=vs-2019
    // Replacing this function ensures we catch any exception that calls terminate (https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/terminate-crt?view=msvc-160)
    // Terminate is normally called when
    // - A matching catch handler cannot be found for a thrown C++ exception
    // - An exception is thrown by a destructor function during stack unwind
    // - The stack is corrupted after throwing an exception
    // Having a custom terminate function allows us to intercept these
    // Thread safe: There is a single set_terminate handler for all dynamically linked DLLs or EXEs; even if you call set_terminate your handler may be replaced by another.
    set_terminate(&terminator);

    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/get-purecall-handler-set-purecall-handler?view=vs-2019
    // This sets the error handler for a pure virtual function call.
    // Thread safe: Because there is only one _purecall_handler for each process, when you call _set_purecall_handler it immediately impacts all threads.
    _set_purecall_handler(&terminator);

    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-invalid-parameter-handler-set-thread-local-invalid-parameter-handler?view=vs-2019
    // This sets the handler if an invalid argument is parsed by the c runtime lib
    // Thread safe: Only one function can be specified as the global invalid argument handler at a time. 
    _set_invalid_parameter_handler(&invalid_parameter_handler);

    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-new-handler?view=vs-2019
    // Transfers control to your error-handling mechanism if the new operator fails to allocate memory. 
    // Thread safe: There is a single _set_new_handler handler for all dynamically linked DLLs or EXEs; even if you call _set_new_handler your handler might be replaced by another.
    _set_new_handler(&memory_depleted);
    // This sets the handler mode for malloc, so that on failure it will call the error handler (it doesn't by default).
    _set_new_mode(1);

    // https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-setunhandledexceptionfilter
    // Enables an application to supersede the top-level exception handler.
    // If an exception occurs in a process that is not being debugged, and the exception makes it to the unhandled exception filter, that filter will call this function.
    // Thread safe: Issuing SetUnhandledExceptionFilter replaces the existing top-level exception filter for all existing and all future threads in the calling process
    // Notes:
    // https://www.codeproject.com/Articles/154686/SetUnhandledExceptionFilter-and-the-C-C-Runtime-Li
    // https://stackoverflow.com/questions/19656946/why-setunhandledexceptionfilter-cannot-capture-some-exception-but-addvectoredexc
    // The C/C++ Runtime Library will remove any custom crash handler in certain circumstances.
    // However, is will also call AddVectoredExceptionHandler instead in cases where this is removed, so it should be ok
    SetUnhandledExceptionFilter(top_level_exception_filter);

    // https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-addvectoredexceptionhandler?redirectedfrom=MSDN
    // Registers a vectored exception handler.
    // Thread safe: ???
    void* vectored_exception_handler_err = AddVectoredExceptionHandler(1, vectored_exception_handler);
    if (vectored_exception_handler_err == NULL) {
        FATALERROR("Unabled to AddVectoredExceptionHandler");
    }

    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/signal?view=msvc-160
    // Sets the signal handler for a given signal (only one signal handler can be installed per signal)
    // Thread safe: https://stackoverflow.com/questions/12952262/signal-handler-function-in-multithreaded-environment
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGSEGV, signal_handler);

    // Inject into the dark souls panic function and call our handler directly
    uint8_t *write_address = (uint8_t*)(panic_debug_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &panic_debug_return, 1, &panic_debug_injection);
}
