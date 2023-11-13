_DATA SEGMENT

get_EzMenuEzMenuDlgObj_for_namedObj   dq  140ed6020h
MenuDlgObjStr   dw  "i","c","o","n","_","l","e","v","e","l","s","y","n","c",0 ;unicode
loc_1406F6154   dq  1406F6154h

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


PUBLIC Null_New_IconObj_FrpgMenuDlgFloatingPCGauge_injection
Null_New_IconObj_FrpgMenuDlgFloatingPCGauge_injection PROC

;new code (null the new pointer)
mov     [rbx+250h], rax
;original code
mov     [rbx+240h], rax
mov     rax, rbx
add     rsp, 20h
pop     rbx
ret

Null_New_IconObj_FrpgMenuDlgFloatingPCGauge_injection ENDP


EXTERN Insert_IconObj_FrpgMenuDlgFloatingPCGauge_return: qword

PUBLIC Insert_IconObj_FrpgMenuDlgFloatingPCGauge_injection
Insert_IconObj_FrpgMenuDlgFloatingPCGauge_injection PROC

;original code
mov     rcx, rdi
call    qword ptr [get_EzMenuEzMenuDlgObj_for_namedObj]
mov     [rdi+240h], rax

;custom code (get obj and insert)
mov     rcx, rdi
lea     rdx, MenuDlgObjStr
call    qword ptr [get_EzMenuEzMenuDlgObj_for_namedObj]
mov     [rdi+250h], rax

jmp     Insert_IconObj_FrpgMenuDlgFloatingPCGauge_return

Insert_IconObj_FrpgMenuDlgFloatingPCGauge_injection ENDP


EXTERN Render_IconObj_FrpgMenuDlgFloatingPCGauge_return: qword
extern Render_IconObj_FrpgMenuDlgFloatingPCGauge_helper: PROC

PUBLIC Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection
Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection PROC

FUNC_PROLOGUE
mov     rcx, r14 ;the FrpgMenuDlgFloatingPCGauge obj
call    Render_IconObj_FrpgMenuDlgFloatingPCGauge_helper
FUNC_EPILOGUE

;original code
cmp     qword ptr [rbp-9], 8
jnb     continue 
jmp     qword ptr [loc_1406F6154]
continue:
mov     rcx, [rbp-29h]
mov     rax, [rcx]
jmp     Render_IconObj_FrpgMenuDlgFloatingPCGauge_return

Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection ENDP

_TEXT    ENDS

END
