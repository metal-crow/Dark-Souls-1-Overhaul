_DATA SEGMENT

abort_enter dq 14063100fh

_DATA ENDS

_TEXT SEGMENT

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

FUNC_EPILOGUE_NORAX macro
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
    add     rsp, 8
    popfq 
endm

extern equipment_change_unlock_injection_return: qword
EXTERN equipment_change_unlock_injection_helper: PROC

PUBLIC equipment_change_unlock_injection
equipment_change_unlock_injection PROC

FUNC_PROLOGUE
mov     ecx, esi ;the menu slot user wants to change
call    equipment_change_unlock_injection_helper
FUNC_EPILOGUE_NORAX

test    al, al
jz      no_equip_change ;false, so no change allowed

;original code
mov     qword ptr [rsp+144], rbx
mov     qword ptr [rsp+168], rdi
jmp     equipment_change_unlock_injection_return
no_equip_change:
jmp     qword ptr [abort_enter]
equipment_change_unlock_injection ENDP

_TEXT    ENDS

END
