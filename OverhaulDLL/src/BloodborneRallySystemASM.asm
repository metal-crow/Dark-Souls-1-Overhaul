_DATA SEGMENT

_DATA ENDS

_TEXT SEGMENT

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

;save temp registers
sub     rsp, 8 ;make 16 aligned
sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

;call helper function
mov     rdx, rcx ;orange_bar
mov     rcx, r15 ;bar_id
call    control_timer_function
cmp     rax, 1 ;check if we set the orange bar normally or not
je      control_bar_exit

normal_exit:
;restore tmp
add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h
add     rsp, 8

;original code
movss   xmm0, dword ptr [rcx+60h]
subss   xmm0, xmm1
andps   xmm0, xmm5
comiss  xmm6, xmm0
jmp     control_timer_injection_return

control_bar_exit:
;restore tmp
add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h
add     rsp, 8

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

;call helper c code
mov     r9, rdx ;new_hp
lea     r8, [rsp+28h] ;attack_data
add     r8, 8*7 ;account for saved tmp registers
mov     rdx, rcx ;target
mov     rcx, r15 ;attacker
call    main_rally_function

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
jmp     main_rally_injection_return

main_rally_injection ENDP

extern RALLY_EFFECT_ID: dword
extern lua_SetEventSpecialEffect_2: qword

PUBLIC set_rally_regain_sfx
set_rally_regain_sfx PROC

;save temp registers
push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 8h
sub     rsp, 20h

mov     r8d, RALLY_EFFECT_ID
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]

add     rsp, 20h
add     rsp, 8h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax
ret

set_rally_regain_sfx ENDP

extern RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND: dword

PUBLIC set_rally_sfx_rhand
set_rally_sfx_rhand PROC

;save temp registers
push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

mov     r8d, RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax
ret

set_rally_sfx_rhand endp

extern RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND: dword

PUBLIC set_rally_sfx_lhand
set_rally_sfx_lhand PROC

;save temp registers
push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

mov     r8d, RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND
mov     edx, 10000
call    [lua_SetEventSpecialEffect_2]

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax
ret

set_rally_sfx_lhand endp

_TEXT    ENDS

END
