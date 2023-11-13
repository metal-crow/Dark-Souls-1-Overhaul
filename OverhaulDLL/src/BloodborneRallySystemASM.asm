_DATA SEGMENT

_DATA ENDS

_TEXT SEGMENT

FUNC_PROLOGUE_LITE macro
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

FUNC_EPILOGUE_LITE macro
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


extern control_timer_injection_return: qword
extern control_timer_function: proc

;Injected into the function which sets the smoothing/orange aspects of Ui bars
PUBLIC control_timer_injection
control_timer_injection PROC

FUNC_PROLOGUE
mov     rdx, rcx ;orange_bar
mov     rcx, r15 ;bar_id
call    control_timer_function
cmp     rax, 1 ;check if we set the orange bar normally or not
je      control_bar_exit

normal_exit:
FUNC_EPILOGUE

;original code
movss   xmm0, dword ptr [rcx+60h]
subss   xmm0, xmm1
andps   xmm0, xmm5
comiss  xmm6, xmm0
jmp     control_timer_injection_return

control_bar_exit:
FUNC_EPILOGUE

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

FUNC_PROLOGUE
mov     r9, rdx ;new_hp
lea     r8, [rax+20h + 10h] ;attack_data
mov     rdx, rcx ;target
mov     rcx, r15 ;attacker
call    main_rally_function
FUNC_EPILOGUE

jmp     main_rally_injection_return
main_rally_injection ENDP


extern RALLY_EFFECT_ID: dword
extern lua_SetEventSpecialEffect_2: qword

PUBLIC set_rally_regain_sfx
set_rally_regain_sfx PROC

FUNC_PROLOGUE_LITE
mov     r8d, RALLY_EFFECT_ID
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]
FUNC_EPILOGUE_LITE

ret
set_rally_regain_sfx ENDP


extern RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND: dword

PUBLIC set_rally_sfx_rhand
set_rally_sfx_rhand PROC

FUNC_PROLOGUE_LITE
mov     r8d, RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]
FUNC_EPILOGUE_LITE

ret
set_rally_sfx_rhand endp


extern RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND: dword

PUBLIC set_rally_sfx_lhand
set_rally_sfx_lhand PROC

FUNC_PROLOGUE_LITE
mov     r8d, RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]
FUNC_EPILOGUE_LITE

ret
set_rally_sfx_lhand endp

_TEXT    ENDS

END
