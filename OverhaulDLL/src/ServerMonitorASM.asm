_DATA SEGMENT

cstring_from_charptr    dq  14010df50h

_DATA ENDS

_TEXT    SEGMENT

FUNC_PROLOGUE macro
    pushfq 
    push    rax
    mov     rax,rsp
    and     rsp,-10h
    sub     rsp,000002A0h
    fxsave  [rsp+20h]
    mov     [rsp+00000220h],rbx
    mov     [rsp+00000228h],rcx
    mov     [rsp+00000230h],rdx
    mov     [rsp+00000238h],rsi
    mov     [rsp+00000240h],rdi
    mov     [rsp+00000248h],rax
    mov     [rsp+00000250h],rbp
    mov     [rsp+00000258h],r8
    mov     [rsp+00000260h],r9
    mov     [rsp+00000268h],r10
    mov     [rsp+00000270h],r11
    mov     [rsp+00000278h],r12
    mov     [rsp+00000280h],r13
    mov     [rsp+00000288h],r14
    mov     [rsp+00000290h],r15
endm
 
FUNC_EPILOGUE macro
    mov     r15,[rsp+00000290h]
    mov     r14,[rsp+00000288h]
    mov     r13,[rsp+00000280h]
    mov     r12,[rsp+00000278h]
    mov     r11,[rsp+00000270h]
    mov     r10,[rsp+00000268h]
    mov     r9, [rsp+00000260h]
    mov     r8, [rsp+00000258h]
    mov     rbp,[rsp+00000250h]
    mov     rdi,[rsp+00000240h]
    mov     rsi,[rsp+00000238h]
    mov     rdx,[rsp+00000230h]
    mov     rcx,[rsp+00000228h]
    mov     rbx,[rsp+00000220h]
    fxrstor [rsp+20h]
    mov     rsp,[rsp+00000248h]
    pop     rax
    popfq 
endm


EXTERN BreakIn_UserSteamId_injection_helper: PROC
EXTERN BreakIn_UserSteamId_injection_return: qword

PUBLIC BreakIn_UserSteamId_injection
BreakIn_UserSteamId_injection PROC
;original code
MOV     R8,RSI
LEA     RCX,[RBP + 7]
CALL    qword ptr [cstring_from_charptr]

mov     RCX, qword ptr [rax]
FUNC_PROLOGUE
call    BreakIn_UserSteamId_injection_helper
FUNC_EPILOGUE

;original code
LEA     RCX,[R15 + 68h]
jmp     BreakIn_UserSteamId_injection_return
BreakIn_UserSteamId_injection ENDP

_TEXT    ENDS

END
