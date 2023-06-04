_DATA SEGMENT

_DATA ENDS

_TEXT SEGMENT

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

FUNC_PROLOGUE_USER14 macro
	push	r14
	mov		r14, rsp
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
	mov		[rsp + 20h], r14
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

FUNC_EPILOGUE_USER14 macro
	mov		r14, [rsp + 20h]
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
	mov		rsp, r14
	pop		r14
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

extern beforehit_time: qword

extern weapon_toggle_injection_return: qword

;injected into the function that is triggered on weapon toggle
PUBLIC weapon_toggle_injection
weapon_toggle_injection PROC

mov  qword ptr [beforehit_time], 0 ; Fake we got hit infinite time ago, to drop rally

;original code
mov  r9, 140195870h
mov  edx, 1
mov  rcx, 141c6a8e8h
mov  rcx, QWORD PTR [rcx]
jmp  weapon_toggle_injection_return

weapon_toggle_injection ENDP

extern control_timer_injection_return: qword
extern control_timer_function: proc

;Injected into the function which sets the smoothing/orange aspects of Ui bars
PUBLIC control_timer_injection
control_timer_injection PROC

FUNC_PROLOGUE_USER14
mov     rdx, rcx ;orange_bar
mov     rcx, r15 ;bar_id
call    control_timer_function
cmp     rax, 1 ;check if we set the orange bar normally or not
je      control_bar_exit

normal_exit:
FUNC_EPILOGUE_USER14

;original code
movss   xmm0, dword ptr [rcx+60h]
subss   xmm0, xmm1
andps   xmm0, xmm5
comiss  xmm6, xmm0
jmp     control_timer_injection_return

control_bar_exit:
FUNC_EPILOGUE_USER14

;set the SSE regs to the desired value
movss   xmm2, dword ptr [rcx+60h]
movss   xmm1, dword ptr [rcx+60h]

;original code
movss   xmm0, dword ptr [rcx+60h]
subss   xmm0, xmm1
andps   xmm0, xmm5
comiss  xmm6, xmm0
jmp     control_timer_injection_return

control_timer_injection ENDP

extern main_rally_injection_return: qword
extern main_rally_function: proc

;Inject into function that is called on hit
PUBLIC main_rally_injection
main_rally_injection PROC

;original code
mov     edx, [rbx+3E8h]
MOV     RCX,RBX
add     edx, edi

FUNC_PROLOGUE_USER14
mov     r9, rdx ;new_hp
lea     r8, [r14+20h + 8] ;attack_data
mov     rdx, rcx ;target
mov     rcx, r15 ;attacker
call    main_rally_function
FUNC_EPILOGUE_USER14

jmp     main_rally_injection_return

main_rally_injection ENDP

extern RALLY_EFFECT_ID: dword
extern lua_SetEventSpecialEffect_2: qword

PUBLIC set_rally_regain_sfx
set_rally_regain_sfx PROC

FUNC_PROLOGUE
mov     r8d, RALLY_EFFECT_ID
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]
FUNC_EPILOGUE

ret

set_rally_regain_sfx ENDP

extern RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND: dword

PUBLIC set_rally_sfx_rhand
set_rally_sfx_rhand PROC

FUNC_PROLOGUE
mov     r8d, RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]
FUNC_EPILOGUE

ret

set_rally_sfx_rhand endp

extern RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND: dword

PUBLIC set_rally_sfx_lhand
set_rally_sfx_lhand PROC

FUNC_PROLOGUE
mov     r8d, RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]
FUNC_EPILOGUE

ret

set_rally_sfx_lhand endp

_TEXT    ENDS

END
