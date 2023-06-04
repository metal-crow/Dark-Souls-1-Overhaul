_DATA SEGMENT

get_EzMenuEzMenuDlgObj_for_namedObj   dq  140ed6020h
MenuDlgObjStr   dw  "i","c","o","n","_","l","e","v","e","l","s","y","n","c",0 ;unicode
loc_1406F6154   dq  1406F6154h

_DATA ENDS

_TEXT    SEGMENT

FUNC_PROLOGUE macro
	push	r15
	mov		r15, rsp
	and		rsp, -10h
	sub		rsp, 0C0h
	movaps	[rsp + 0B0h], xmm0
	movaps	[rsp + 0A0h], xmm1
	movaps	[rsp + 90h], xmm2
	movaps	[rsp + 80h], xmm3
	movaps	[rsp + 70h], xmm4
	movaps	[rsp + 60h], xmm5
	mov		[rsp + 58h], rax
	mov		[rsp + 50h], rcx
	mov		[rsp + 48h], rdx
	mov		[rsp + 40h], r8
	mov		[rsp + 38h], r9
	mov		[rsp + 30h], r10
	mov		[rsp + 28h], r11
	mov		[rsp + 20h], r15
endm

FUNC_EPILOGUE macro
	mov		r15, [rsp + 20h]
	mov		r11, [rsp + 28h]
	mov		r10, [rsp + 30h]
	mov		r9, [rsp + 38h]
	mov		r8, [rsp + 40h]
	mov		rdx, [rsp + 48h]
	mov		rcx, [rsp + 50h]
	mov		rax, [rsp + 58h]
	movaps	xmm5, [rsp + 60h]
	movaps	xmm4, [rsp + 70h]
	movaps	xmm3, [rsp + 80h]
	movaps	xmm2, [rsp + 90h]
	movaps	xmm1, [rsp + 0A0h]
	movaps	xmm0, [rsp + 0B0h]
	mov		rsp, r15
	pop		r15
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
