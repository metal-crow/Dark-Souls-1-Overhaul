_DATA SEGMENT

_DATA ENDS

_TEXT SEGMENT

extern panic_debug_injection_helper_function: PROC
extern panic_debug_return: qword

PUBLIC panic_debug_injection
panic_debug_injection PROC

call    panic_debug_injection_helper_function

;original code
add     rsp, 60h
jmp     panic_debug_return

panic_debug_injection ENDP

_TEXT    ENDS

END
