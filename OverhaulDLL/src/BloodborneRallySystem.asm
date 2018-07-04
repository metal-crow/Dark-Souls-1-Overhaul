_DATA SEGMENT

control_timer_jump dq 14065F8C3h

_DATA ENDS

_TEXT SEGMENT

extern beforehit_time: qword
extern beforehit_hp: qword
extern current_selected_bar: qword
extern gothit: qword
extern rally_system_WorldChrBase: qword
extern MAX_RALLY_RECOVERY_TIME_MS: qword
extern time_ms_ptr: qword

extern weapon_toggle_injection_return: qword

;injected into the function that is triggered on weapon toggle
PUBLIC weapon_toggle_injection
weapon_toggle_injection PROC

mov  qword ptr [beforehit_time], 0 ; Fake we got hit infinite time ago, to drop rally
mov  r9, 14018F920h
mov  edx, 1
mov  rcx, 141CF3720h
mov  rcx, QWORD PTR [rcx]
jmp  weapon_toggle_injection_return

weapon_toggle_injection ENDP

extern current_selected_bar_injection_return: qword

;Injected into the function which loops over all UI bars to be edited
PUBLIC selected_bar_injection
selected_bar_injection PROC

mov  dword ptr [current_selected_bar], r15d
inc  r15d
add  rcx, 8
mov  qword ptr [rbp-80h], rcx
cmp  r15d, [rsi+20h]
jmp current_selected_bar_injection_return

selected_bar_injection ENDP

extern control_timer_injection_return: qword

;Injected into the function which sets the smoothing/orange aspects of Ui bars
PUBLIC control_timer_injection
control_timer_injection PROC

;part of original code
subss   xmm2, xmm3

;if this is the timer of interest
cmp     qword ptr [current_selected_bar], 23h
je      start_controlling_timer
cmp     qword ptr [current_selected_bar], 24h
je      start_controlling_timer
cmp     qword ptr [current_selected_bar], 25h
je      start_controlling_timer
cmp     qword ptr [current_selected_bar], 26h
je      start_controlling_timer
jmp     dont_control_timer

start_controlling_timer:
; Fix for coming into section with too many hits;
;     can happen if pc gets hit but doesn't take hp _bar_ damage (since HP can extend past visual)
;     causing this code not to get run immediatly after hit
cmp     [gothit], 2
jle     timer_check_continue
mov     [gothit], 1

timer_check_continue:
; Check timer
push    rax
mov     rax, [beforehit_time]
add     rax, MAX_RALLY_RECOVERY_TIME_MS
push    rbx
mov     rbx, [time_ms_ptr]
mov     ebx, dword ptr [rbx]
cmp     ebx, eax ; Compare current time to hittime+MAX RECOVERY TIME
pop     rbx
pop     rax
jge     execute_orange_drop

; If we got hit before the timer went down
cmp     [gothit], 2
je      execute_partial_orange_drop
jmp     safe_exit

; If timer is at desired point, drop orange
execute_orange_drop:
mov     [gothit], 0
mov     DWORD PTR [rcx + 60h], 0
jmp     safe_exit

; If we got interrupted before previous drop, drop orange to the _previous_ hp value
execute_partial_orange_drop:
mov     [gothit], 1
; Save sse's
sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
; Calculate previous_hp/max_hp. This should be location of orange
;   NOTE: Max HP must be capped at _displayable_ max hp, otherwise get problems. However, we patched that bug out, so no check is fine
push    rax
mov     rax, QWORD PTR [rally_system_WorldChrBase]
mov     rax, [rax]
mov     rax, [rax + 68h]
mov     eax, dword ptr [rax + 3DCh] ; Max HP
movd    xmm0, eax
pop     rax
movss   xmm1, dword ptr [beforehit_hp]
divss   xmm1, xmm0
movss   dword ptr [rcx+60h], xmm1
; Restore sse's
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h
jmp     safe_exit

; If timer not interested, normal instruction
dont_control_timer:
comiss  xmm1, xmm2
movss   dword ptr [rcx+60h], xmm2
jnb     branch_a
jmp     qword ptr [control_timer_jump]
branch_a:
jmp     control_timer_injection_return

safe_exit:
comiss  xmm1, xmm2
jnb     branch_b
jmp      qword ptr [control_timer_jump]
branch_b:
jmp     control_timer_injection_return

control_timer_injection ENDP

extern main_rally_injection_return: qword
extern main_rally_function: proc
extern RALLY_EFFECT_ID: dword
extern lua_SetEventSpecialEffect_2: qword

;Inject into function that is called on hit
PUBLIC main_rally_injection
main_rally_injection PROC

;call helper c code
push rdx ;new_hp
push rsp ;attack_data
push rcx ;target
push r15 ;attacker
call main_rally_function
cmp eax, edx
je normal_exit
mov edx, eax ;set new hp
;set rally sfx
mov     edx, RALLY_EFFECT_ID ;a2
mov     r8d, 10000 ;a3, character id
call    [lua_SetEventSpecialEffect_2]
;exit
normal_exit:
mov     [rsp+10h], edx
push    rbx
sub     rsp, 20h
test    byte ptr [rcx+514h], 40h
jmp main_rally_injection_return

main_rally_injection ENDP

extern RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND: dword

PUBLIC set_rally_sfx_rhand
set_rally_sfx_rhand PROC

mov     edx, RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND
mov     r8d, 10000
call    [lua_SetEventSpecialEffect_2]

set_rally_sfx_rhand endp

extern RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND: dword

PUBLIC set_rally_sfx_lhand
set_rally_sfx_lhand PROC

mov     edx, RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND
mov     r8d, 10000
call    [lua_SetEventSpecialEffect_2]

set_rally_sfx_lhand endp

_TEXT    ENDS

END
