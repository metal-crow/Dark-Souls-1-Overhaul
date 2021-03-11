/*
 * This specific file is released under the MIT License
 * Copyright(c) 2020 Metal-Crow.
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "CrashHandler.h"
#include "DllMain.h"
#include "StackWalker.h"

#include <eh.h>
#include <new.h>
#include <signal.h>
#include <Windows.h>
#include <direct.h>
#include <ctime>
#include <stdint.h>
#include <string>

/* ------------------ Helpers ------------------ */

std::string exception_record_tostring(struct _EXCEPTION_RECORD *ExceptionRecord)
{
    std::string out;
    char scratch[128];

    snprintf(scratch, sizeof(scratch), "-------------------\nExceptionRecord:\n");
    out.append(scratch);

    snprintf(scratch, sizeof(scratch), "ExceptionCode=%u\nExceptionFlags=%u\n", ExceptionRecord->ExceptionCode, ExceptionRecord->ExceptionFlags);
    out.append(scratch);

    if (ExceptionRecord->ExceptionRecord != NULL)
    {
        std::string recursed_str = exception_record_tostring(ExceptionRecord->ExceptionRecord);
        out.append(recursed_str);
    }

    snprintf(scratch, sizeof(scratch), "ExceptionAddress=%llx\nNumberParameters=%u\n", (uint64_t)ExceptionRecord->ExceptionAddress, ExceptionRecord->NumberParameters);
    out.append(scratch);

    for (size_t i = 0; i < ExceptionRecord->NumberParameters; i++)
    {
        snprintf(scratch, sizeof(scratch), "ExceptionRecord[%zd]=%lx\n", i, ExceptionRecord->ExceptionInformation[i]);
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
    snprintf(scratch, sizeof(scratch), "Dr0=%lx", ContextRecord->Dr0);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr1=%lx", ContextRecord->Dr1);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr2=%lx", ContextRecord->Dr2);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr3=%lx", ContextRecord->Dr3);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr6=%lx", ContextRecord->Dr6);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Dr7=%lx", ContextRecord->Dr7);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rax=%lx", ContextRecord->Eax);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rcx=%lx", ContextRecord->Ecx);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rdx=%lx", ContextRecord->Edx);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rbx=%lx", ContextRecord->Ebx);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rsp=%lx", ContextRecord->Esp);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rbp=%lx", ContextRecord->Ebp);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rsi=%lx", ContextRecord->Esi);
    out.append(scratch);
    snprintf(scratch, sizeof(scratch), "Rdi=%lx", ContextRecord->Edi);
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
    std::string GetOutput()
    {
        //clean the buffer between callstack requests
        std::string output(output_text);
        output_text.clear();
        return output;
    }
protected:
    std::string output_text;
    virtual void OnOutput(LPCSTR szText)
    {
        //this is called multiple times from ShowCallstack, so need it to save and append
        output_text.append(szText);
    }

};

/* ------------------ Handlers ------------------ */

bool triggered_crash_handler = false;

void crash_handler(char* message_str)
{
    // Prevent an internal failure -> abort from infinitely recursing, and also catch any unexpected double-faults
    signal(SIGABRT, NULL);
    if (triggered_crash_handler)
    {
        abort();
    }
    triggered_crash_handler = true;

    // Create the locations to output the dump to
    char output_dir[MAX_PATH + 100];
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

    // Save the stack trace
    MyStackWalker sw;
    sw.ShowCallstack(); //This interally calls OnOutput, which we overwrite to only save the output
    std::string stack_info = sw.GetOutput(); // Get the saved output

    // Write out the stack trace and any message info
    FILE* fp;
    char msg_file[sizeof(output_dir)];
    snprintf(msg_file, sizeof(msg_file), "%s\\%s", output_dir, "message");
    fopen_s(&fp, msg_file, "w");
    fprintf(fp, "%s\n\n", stack_info.c_str());
    if (message_str != NULL)
    {
        fprintf(fp, "%s", message_str);
    }
    fclose(fp);

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
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x406d1388 || ExceptionInfo->ExceptionRecord->ExceptionCode == 0xe06d7363)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    /* Notes:
     * I have also encountered other standard windows exceptions being raised for unclear reasons.
     * https://stackoverflow.com/questions/12298406/how-to-treat-0x40010006-exception-in-vectored-exception-handler specifies that some of these are expected to be ignored.
     * To error on the side of safety, only catch exceptions starting with 0xC0
     */
    if ((ExceptionInfo->ExceptionRecord->ExceptionCode & 0xFF000000) != 0xC0000000)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    crash_handler((char*)exception_pointer_tostring(ExceptionInfo)->c_str());
    return EXCEPTION_CONTINUE_SEARCH;
}

/* ------------------ Main Function ------------------ */

void set_crash_handlers()
{
    print_console(Mod::output_prefix + "Adding crash handlers...\n");

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

    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/signal?view=msvc-160
    // Sets the signal handler for a given signal (only one signal handler can be installed per signal)
    // Thread safe: https://stackoverflow.com/questions/12952262/signal-handler-function-in-multithreaded-environment
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGSEGV, signal_handler);
}
