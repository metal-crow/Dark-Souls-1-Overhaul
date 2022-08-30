_DATA SEGMENT

aUnnamed_1      dq  1414A1F20h

_DATA ENDS

_TEXT SEGMENT

extern calculate_attack_damage_injection_return: qword
extern last_attack_weaponid: dword

PUBLIC calculate_attack_damage_injection
calculate_attack_damage_injection PROC

;save the weapon id from the struct
mov         eax, dword ptr [RSI + 048h] ;weapon id
mov         last_attack_weaponid, eax

;original code
CVTTSS2SI   EAX,XMM6
mov         dword ptr [RSI + 17Ch], EAX
cvttss2si   eax, dword ptr [rbp-68h]

jmp     calculate_attack_damage_injection_return

calculate_attack_damage_injection ENDP

extern char_loaded: byte

extern char_loaded_injection_return: qword

PUBLIC char_loaded_injection
char_loaded_injection PROC

;write out to our global that we're loaded
mov     char_loaded, 1

;original code
mov     [rsp+20h], rbx
push    rsi
push    rdi
push    r14
sub     rsp, 60h
mov     rax, qword ptr [141B47800h]

jmp     char_loaded_injection_return

char_loaded_injection ENDP

extern char_loading_injection_return: qword

PUBLIC char_loading_injection
char_loading_injection PROC

;write out to our global that we're loading
mov     char_loaded, 0

;original code
push    rdi
sub     rsp, 30h
mov     qword ptr [rsp+20h], -2

jmp     char_loading_injection_return

char_loading_injection ENDP


extern gui_hpbar_max_injection_return: qword
extern Gui_HP_bar_UI_ptr: qword

PUBLIC gui_hpbar_max_injection
gui_hpbar_max_injection PROC
;save the pointer to the HP bar ui size
mov     Gui_HP_bar_UI_ptr, rbx
add     Gui_HP_bar_UI_ptr, 504h
;original code
mov     dword ptr [rbx+508h], 44220000h
mov     qword ptr [rbx+50Ch], 43C80000h
jmp     gui_hpbar_max_injection_return
gui_hpbar_max_injection ENDP

extern stop_durability_damage_injection_return: qword
extern stop_durability_damage_original_jump: qword

PUBLIC stop_durability_damage_hook
stop_durability_damage_hook PROC

    cmp dword ptr[r11+14h], r9d                     ; if the new durability value is NOT lower than the current value
    jle originalcode                                ;     we don't need to change the new value since it's the same or large than the current one
    mov r9d, dword ptr[r11+14h]                     ; else: set the new durability value to the current one (freezing it)
    
    originalcode:                                   ; since the long jump overwrites part of the function it is added here
                                                    ; the long jump overwrites the following four instructions
        mov dword ptr [r11+14h], r9d                ; updates the durability value (to new value - which was set to the current value)
        cmp r10d, dword ptr[rcx+20h]                ; instruction that was overwritten
        jge hopper                                  ; this was tricky, since the relative jump was overwritten we calculate the address it would have jumped to (stop_durability_damage_original_jump)
        mov rax, qword ptr[rcx+20h]                 ; last instruciton that was overwritten
        jmp stop_durability_damage_injection_return ; jump back to the original code (right after the long jump)

    hopper:
        jmp stop_durability_damage_original_jump    ; had to use a hopper & jmp as conditional jumps only support relative mode

stop_durability_damage_hook ENDP


EXTERN grab_movemapstep_return: qword
EXTERN grab_movemapstep_value: qword

PUBLIC grab_movemapstep_injection
grab_movemapstep_injection PROC
mov qword ptr [grab_movemapstep_value], rcx
;original code
mov     [rsp+8], rcx
push    rbx
push    rbp
push    rsi
push    rdi
push    r14
sub     rsp, 30h
jmp     grab_movemapstep_return
grab_movemapstep_injection ENDP


EXTERN grab_thread_handle_return: qword
extern grab_thread_handle_helper: proc

PUBLIC grab_thread_handle_injection
grab_thread_handle_injection PROC
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

mov     rcx, qword ptr [rsi + 8h] ;handle
call    grab_thread_handle_helper

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
mov     [rsi+10h], eax
mov     rdx, aUnnamed_1
test    rdi, rdi
cmovnz  rdx, rdi
jmp     grab_thread_handle_return
grab_thread_handle_injection ENDP

_TEXT    ENDS

END
