_DATA   SEGMENT

LAB_140e2af07   dq  140e2af07h

_DATA   ENDS

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

;hook XINPUT get state call
EXTERN XInput_Get_State_inject_return: qword
EXTERN intercept_xinput_get_state: PROC

PUBLIC XInput_Get_State_inject
XInput_Get_State_inject PROC

jmp    intercept_xinput_get_state

XInput_Get_State_inject ENDP


EXTERN intercept_IDirectInputDevice8GetDeviceState_Keyboard: PROC
EXTERN IDirectInputDevice8GetDeviceState_Keyboard_injection_return: qword

PUBLIC IDirectInputDevice8GetDeviceState_Keyboard_injection
IDirectInputDevice8GetDeviceState_Keyboard_injection PROC

;verify the return value is ok
cmp     eax, 0
jne     exit

FUNC_PROLOGUE
lea     rcx, [rbx+1B0h+8] ;the 1st qword is not keyboard data
call    intercept_IDirectInputDevice8GetDeviceState_Keyboard
FUNC_EPILOGUE

;original code
exit:
cmp     eax, 8007001Eh
jz      continue
jmp     qword ptr [LAB_140e2af07]
continue:
mov     rcx, [rbx+1B0h]

jmp     IDirectInputDevice8GetDeviceState_Keyboard_injection_return

IDirectInputDevice8GetDeviceState_Keyboard_injection ENDP


EXTERN intercept_IDirectInputDevice8GetDeviceState_DIJOYSTATE2: PROC
EXTERN IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection_return: qword

PUBLIC IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection
IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection PROC

FUNC_PROLOGUE
lea     rcx, [RDI + 5f0h + 8]
call    intercept_IDirectInputDevice8GetDeviceState_DIJOYSTATE2
FUNC_EPILOGUE

;original code
movaps  xmmword ptr [rsp+30h], xmm6
lea     rdx, [rbx+180h]
movaps  xmmword ptr [rsp+20h], xmm7

jmp     IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection_return

IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection ENDP

_TEXT    ENDS

END
