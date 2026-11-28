_DATA SEGMENT

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


EXTERN Send_Type17_GeneralRequestTask_injection_helper: PROC
EXTERN Send_Type17_GeneralRequestTask_injection_return: qword

PUBLIC Send_Type17_GeneralRequestTask_injection
Send_Type17_GeneralRequestTask_injection PROC

FUNC_PROLOGUE
mov     rcx, r8
call    Send_Type17_GeneralRequestTask_injection_helper
FUNC_EPILOGUE

;original code
push    rbp
push    rsi
push    rdi
lea     rbp, [rsp-47h]
sub     rsp, 0D0h
jmp     Send_Type17_GeneralRequestTask_injection_return
Send_Type17_GeneralRequestTask_injection ENDP


EXTERN BreakInPushDispatch_injection_helper: PROC
EXTERN BreakInPushDispatch_injection_return: qword

PUBLIC BreakInPushDispatch_injection
BreakInPushDispatch_injection PROC

FUNC_PROLOGUE
call    BreakInPushDispatch_injection_helper
mov     [rsp + 58h], rax ;return the result through the saved rax
FUNC_EPILOGUE
test    al, al
jz      BreakInPushDispatch_original
;the push is being held, return without dispatching it
ret
BreakInPushDispatch_injection ENDP


;Call the dispatcher without going through the injection
PUBLIC BreakInPushDispatch_original
BreakInPushDispatch_original PROC

;original code
push    rbp
push    rsi
push    rdi
push    r14
push    r15
lea     rbp, [rsp-37h]
sub     rsp, 0A0h
jmp     BreakInPushDispatch_injection_return
BreakInPushDispatch_original ENDP


_TEXT    ENDS

END
