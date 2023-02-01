_DATA SEGMENT

get_EzMenuEzMenuDlgObj_for_namedObj   dq  140ed6020h
MenuDlgObjStr   dw  "i","c","o","n","_","l","e","v","e","l","s","y","n","c",0 ;unicode
loc_1406F6154   dq  1406F6154h

_DATA ENDS

_TEXT    SEGMENT

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

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 28h

mov     rcx, r14 ;the FrpgMenuDlgFloatingPCGauge obj
call    Render_IconObj_FrpgMenuDlgFloatingPCGauge_helper

add     rsp, 28h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

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
