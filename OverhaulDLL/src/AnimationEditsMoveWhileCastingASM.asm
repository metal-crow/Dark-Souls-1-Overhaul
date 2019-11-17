_DATA SEGMENT

move_check_1_jmp1   dq  140382D1Fh
move_check_1_jmp2   dq  140382D01h
move_check_2_jmp1   dq  1403A6129h

_DATA ENDS

_TEXT SEGMENT

extern move_check_helper: proc

extern move_check_1_injection_return: qword

PUBLIC move_check_1_injection
move_check_1_injection PROC

push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11

mov     rcx, qword ptr [rdi+20h] ;playerctrl->animationMediator
call    move_check_helper
cmp     eax, 1
jne     noset
mov     sil, 1
noset:

pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax

;original code
test    sil, sil ;this check is what stops movement during some animations
jnz     next1
jmp     qword ptr [move_check_1_jmp1]
next1:
cmp     byte ptr [rbp+7Fh], 0
jnz     next2
jmp     qword ptr [move_check_1_jmp2]
next2:
mov     rax, [rdi+308h]

jmp     move_check_1_injection_return

move_check_1_injection ENDP

extern move_check_2_injection_return: qword

PUBLIC move_check_2_injection
move_check_2_injection PROC

push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11

mov     rcx, [rdi+10h] ;walktwist->playerctrl
mov     rcx, qword ptr [rcx+20h] ;playerctrl->animationMediator
call    move_check_helper
cmp     eax, 1
jne     noset
mov     dword ptr [rdi+140h], 0 ;set this to 0 if we want to move
noset:

pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax

;original code
cmp     dword ptr [rdi+140h], 0FFFFFFFFh
jz      next1
jmp     qword ptr [move_check_2_jmp1]
next1:
cmp     [rdi+25Dh], bl

jmp     move_check_2_injection_return

move_check_2_injection ENDP

extern move_check_3_injection_return: qword

PUBLIC move_check_3_injection
move_check_3_injection PROC

;original code
mov     rax, [rdi+10h]
mov     rcx, [rax+48h]

push    rax
push    rdx
push    r8
push    r9
push    r10
push    r11
push    rcx

mov     rcx, qword ptr [rax+20h] ;playerctrl->animationMediator
call    move_check_helper
pop     rcx ;unpop rcx so its original value in the func is usable here
cmp     eax, 1
jne     noset
mov     dword ptr [rcx+458h], 0 ;set this to 0 if we want to move
noset:

pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rax

;original code
mov     edx, [rcx+458h]

jmp     move_check_3_injection_return

move_check_3_injection ENDP

_TEXT    ENDS

END
