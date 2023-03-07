_DATA SEGMENT

sub_1401862A0   dq  1401862A0h
nullsub_78     dq  140185470h

_DATA ENDS

_TEXT    SEGMENT

EXTERN sendNetMessage_return: qword
extern sendNetMessage_helper: proc

PUBLIC sendNetMessage_injection
sendNetMessage_injection PROC
;original code
mov     rax, rsp
push    rdi
push    r12
push    r13
push    r14
push    r15
sub     rsp, 80h
mov     qword ptr [rax-78h], -2
mov     [rax+10h], rbx
mov     [rax+18h], rbp
mov     [rax+20h], rsi

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

call    sendNetMessage_helper
;we can clobber rax here

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

;check if we abort this call or not
test    al, al
jnz     normal
lea     r11, [rsp+80h]
mov     rbx, [r11+38h]
mov     rbp, [r11+40h]
mov     rsi, [r11+48h]
mov     rsp, r11
pop     r15
pop     r14
pop     r13
pop     r12
pop     rdi
xor     al, al ;aborting call, so return false
ret

;original code
normal:
jmp     sendNetMessage_return
sendNetMessage_injection ENDP


EXTERN getNetMessage_return: qword
extern getNetMessage_helper: proc

PUBLIC getNetMessage_injection
getNetMessage_injection PROC
;original code
mov     [rsp+8], rbx
mov     [rsp+10h], rbp
mov     [rsp+18h], rsi
mov     [rsp+20h], rdi
push    r14
sub     rsp, 20h

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

call    getNetMessage_helper

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

;check if we abort this call or not
test    al, al
jnz     normal
mov     rbx, [rsp+30h]
mov     rbp, [rsp+38h]
mov     rsi, [rsp+40h]
mov     rdi, [rsp+48h]
add     rsp, 20h
pop     r14
xor     eax, eax ;aborting call, so return 0 bytes
ret

normal:
jmp     getNetMessage_return
getNetMessage_injection ENDP


EXTERN send_generalplayerinfo_return: qword
extern send_generalplayerinfo_helper: proc

PUBLIC send_generalplayerinfo_injection
send_generalplayerinfo_injection PROC
;original code
mov     r11, rsp
push    rbp
push    rsi
lea     rbp, [r11-5Fh]
sub     rsp, 0B8h

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

mov     rcx, qword ptr [rdx+10h] ;pointer to the PlayerIns parent from PlayerCtrl
call    send_generalplayerinfo_helper

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

jmp     send_generalplayerinfo_return
send_generalplayerinfo_injection ENDP


EXTERN Read_GeneralPlayerData_return: qword
extern Read_GeneralPlayerData_helper: proc

PUBLIC Read_GeneralPlayerData_injection
Read_GeneralPlayerData_injection PROC
;original code
mov     r11, rsp
push    rbp
push    rdi
push    r15
lea     rbp, [r11-5Fh]
sub     rsp, 0D0h

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
;push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

;NetworkManipulator is passed in implicitly as rcx
call    Read_GeneralPlayerData_helper
;rax gets set to a const afterwards, so we can clobber it here

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
;pop     rax
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

;the game for some reason still processes stuff from type1 even if it doesn't get any packet
;need to exit the function if we're in rollback netcode mode
cmp     al, 0
jne     no_rollback
ADD     RSP, 0D0h
POP     R15
POP     RDI
POP     RBP
RET
no_rollback:
jmp     Read_GeneralPlayerData_return
Read_GeneralPlayerData_injection ENDP


EXTERN disableType18PacketEnforcement_return: qword
extern disableType18PacketEnforcement_helper: proc

PUBLIC disableType18PacketEnforcement_injection
disableType18PacketEnforcement_injection PROC
;original code
push    rbx
sub     rsp, 20h

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
;push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

;playerins passed in implicitly as rcx
call    disableType18PacketEnforcement_helper
;can clobber rax since it's nativly restored afterwards

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
;pop     rax
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

cmp     rax, 0
jne     normal_exit

forced_check_exit:
add     rsp, 20h
pop     rbx
ret

normal_exit:
;original code
mov     rax, [rcx]
mov     rbx, rcx
call    qword ptr [rax+140h]
jmp     disableType18PacketEnforcement_return
disableType18PacketEnforcement_injection ENDP


EXTERN fixPhantomBulletGenIssue_return: qword
extern fixPhantomBulletGenIssue_helper: proc

PUBLIC fixPhantomBulletGenIssue_injection
fixPhantomBulletGenIssue_injection PROC
;original code
mov     rax, [rax]
mov     rcx, rbx

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
;push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

;playerins passed in implicitly as rcx
call    fixPhantomBulletGenIssue_helper
;rax will be either used, or fixed up

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
;pop     rax
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

cmp     al, 1
jne     normal_exit

force_true_exit:
test    al, al
jmp     fixPhantomBulletGenIssue_return

normal_exit:
mov     rax, [rcx]
;original code
call    qword ptr [rax+148h]
test    al, al
jmp     fixPhantomBulletGenIssue_return

fixPhantomBulletGenIssue_injection ENDP


EXTERN rollback_game_frame_sync_inputs_return: qword
extern rollback_game_frame_sync_inputs_helper: proc

PUBLIC rollback_game_frame_sync_inputs_injection
rollback_game_frame_sync_inputs_injection PROC
;original code
call    qword ptr [sub_1401862A0]
mov     rcx, rax
mov     edx, 2
call    qword ptr [nullsub_78]

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

call    rollback_game_frame_sync_inputs_helper

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

jmp     rollback_game_frame_sync_inputs_return
rollback_game_frame_sync_inputs_injection ENDP


EXTERN dsr_frame_finished_return: qword
extern dsr_frame_finished_helper: proc

PUBLIC dsr_frame_finished_injection
dsr_frame_finished_injection PROC
;original code
movzx   ecx, dil
xor     edx, edx
test    al, al
cmovnz  ecx, edx
movzx   eax, cl

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

call    dsr_frame_finished_helper

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

jmp     dsr_frame_finished_return
dsr_frame_finished_injection ENDP

_TEXT    ENDS

END
