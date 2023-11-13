_DATA SEGMENT

jnz_one dq 14032930eh
jmp_two dq 140334513h

_DATA ENDS

_TEXT SEGMENT

FUNC_PROLOGUE_R15 macro
    pushfq 
    push    r15
    mov     r15,rsp
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

FUNC_EPILOGUE_R15_NORBX macro
    mov     r14,[rsp+00000288h]
    mov     r13,[rsp+00000280h]
    mov     r12,[rsp+00000278h]
    mov     r11,[rsp+00000270h]
    mov     r10,[rsp+00000268h]
    mov     r9, [rsp+00000260h]
    mov     r8, [rsp+00000258h]
    mov     rbp,[rsp+00000250h]
    mov     rax,[rsp+00000248h]
    mov     rdi,[rsp+00000240h]
    mov     rsi,[rsp+00000238h]
    mov     rdx,[rsp+00000230h]
    mov     rcx,[rsp+00000228h]
    ;mov     rbx,[rsp+00000220h]
    fxrstor [rsp+20h]
    mov     rsp,[rsp+00000290h]
    pop     r15
    popfq 
endm


extern main_dead_angle_injection_return: qword
extern main_dead_angle_helper: PROC

PUBLIC main_dead_angle_injection
main_dead_angle_injection PROC

FUNC_PROLOGUE_R15
mov     rdx, r12 ;attacker
mov     rcx, r14 ;target
call    main_dead_angle_helper
cmp     eax, 0
jne     exit
;alter the test codes
mov     byte ptr [R15+70h + 10h], 0 ;account for the offset from the registers saved on the stack 
mov     bl, 0
exit:
FUNC_EPILOGUE_R15_NORBX

;original code
cmp     [rsp+70h], dil
jz      jmp_case
jmp     qword ptr [jnz_one]
jmp_case:
jmp     qword ptr [jmp_two]
main_dead_angle_injection ENDP

_TEXT    ENDS

END
