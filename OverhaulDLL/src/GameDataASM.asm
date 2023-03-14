_DATA SEGMENT

aUnnamed_1      dq  1414a5f80h
LAB_1401ad106   dq  1401ad106h

_DATA ENDS

_TEXT SEGMENT

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
mov     rax, qword ptr [141aab820h]

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


EXTERN grab_destruct_thread_handle_return: qword
extern grab_destruct_thread_handle_helper: proc

PUBLIC grab_destruct_thread_handle_injection
grab_destruct_thread_handle_injection PROC
;original code
push    rdi
sub     rsp, 30h
mov     qword ptr [rsp+20h], -2

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

mov     rcx, qword ptr [rcx + 8h] ;handle
call    grab_destruct_thread_handle_helper

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

jmp     grab_destruct_thread_handle_return
grab_destruct_thread_handle_injection ENDP


EXTERN Step_PadMan_ReadInputs_return: qword
extern ReadInputs_allowed: byte

PUBLIC Step_PadMan_ReadInputs_injection
Step_PadMan_ReadInputs_injection PROC
;original code
mov     rcx, [rbx+8]
mov     [rbx+2Ch], rbp
test    rcx, rcx
jnz     continue
jmp     qword ptr [LAB_1401ad106]

;custom code to check bool
continue:
cmp     ReadInputs_allowed, 0
jnz     continueAllowed
jmp     qword ptr [LAB_1401ad106]

continueAllowed:
mov     rax, [rcx] ;original code
jmp     Step_PadMan_ReadInputs_return
Step_PadMan_ReadInputs_injection ENDP


EXTERN Step_QInputMgrWindowsFantasy_ReadInputs_return: qword

PUBLIC Step_QInputMgrWindowsFantasy_ReadInputs_injection
Step_QInputMgrWindowsFantasy_ReadInputs_injection PROC
;original code
mov     rax, [rcx]
lea     rdx, [rsp+80h]

;custom code to check bool
cmp     ReadInputs_allowed, 0
jz      skip_read
call    qword ptr [rax+18h] ;original code
skip_read:
jmp     Step_QInputMgrWindowsFantasy_ReadInputs_return
Step_QInputMgrWindowsFantasy_ReadInputs_injection ENDP

_TEXT    ENDS

END
