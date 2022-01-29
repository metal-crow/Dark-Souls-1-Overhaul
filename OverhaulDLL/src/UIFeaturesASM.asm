_DATA SEGMENT

get_EzMenuEzMenuDlgObj_for_namedObj   dq  140ED2440h
MenuDlgObjStr   dw  "i","c","o","n","_","l","e","v","e","l","s","y","n","c",0 ;unicode
FloatingMultiplayHpBar_ShowIcon   dq  140ED81D0h
FUN_14031ff30   dq  14031ff30h
loc_1406F352A   dq  1406F352Ah

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

PUBLIC Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection
Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection PROC
;custom code
mov     rcx, [r14+250h] ;the custom icon obj
test    rcx, rcx
jz      noNewIconRender
mov     edx, 1 ;if we should render the icon
call    qword ptr [FloatingMultiplayHpBar_ShowIcon] ;function to render icon

;original code
noNewIconRender:
mov     rcx, rdi
call    qword ptr [FUN_14031ff30]
cmp     eax, 2
jnz     continue
jmp     qword ptr [loc_1406F352A]
continue:
mov     rcx, rdi

jmp     Render_IconObj_FrpgMenuDlgFloatingPCGauge_return
Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection ENDP

_TEXT    ENDS

END
