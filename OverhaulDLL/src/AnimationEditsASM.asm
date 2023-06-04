_DATA SEGMENT

sub_1404363c0   dq  1404363c0h

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

FUNC_EPILOGUE_NORAX macro
	mov		r15, [rsp + 20h]
	mov		r11, [rsp + 28h]
	mov		r10, [rsp + 30h]
	mov		r9, [rsp + 38h]
	mov		r8, [rsp + 40h]
	mov		rdx, [rsp + 48h]
	mov		rcx, [rsp + 50h]
	movaps	xmm5, [rsp + 60h]
	movaps	xmm4, [rsp + 70h]
	movaps	xmm3, [rsp + 80h]
	movaps	xmm2, [rsp + 90h]
	movaps	xmm1, [rsp + 0A0h]
	movaps	xmm0, [rsp + 0B0h]
	mov		rsp, r15
	pop		r15
endm

EXTERN read_body_aid_injection_helper_function: PROC
EXTERN animation_entry_set_return: qword

PUBLIC animation_entry_set_injection
animation_entry_set_injection PROC

;original code
mov     dword ptr [rax+5Ch], 3F800000h
mov     rax, [rbx+8]
mov     [rax+10h], r14d

FUNC_PROLOGUE
add     rax, 5Ch
mov     rdx, rax ;Animation entry speed ptr
mov     rcx, rdi ;Animation aid ptr
call    read_body_aid_injection_helper_function
FUNC_EPILOGUE

jmp animation_entry_set_return

animation_entry_set_injection ENDP

EXTERN disable_whiff_animations_injection_helper: PROC
EXTERN disable_whiff_animations_injection_return: qword

PUBLIC disable_whiff_animations_injection
disable_whiff_animations_injection PROC

;original code
mov     esi, [rdi+80h]
add     esi, 28h
mov     edx, esi
call    qword ptr [sub_1404363c0]

FUNC_PROLOGUE
mov     cl, al ;pass in the value that determines if we whiff as the arg
call    disable_whiff_animations_injection_helper
FUNC_EPILOGUE_NORAX

jmp     disable_whiff_animations_injection_return

disable_whiff_animations_injection ENDP


_TEXT    ENDS

END
