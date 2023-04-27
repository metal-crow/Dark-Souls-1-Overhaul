_DATA SEGMENT

sub_1401862A0   dq  1401862A0h
nullsub_78     dq  140185470h
FUN_140391370   dq  140391370h
PadMan_GetPadDevice dq  1401af490h

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


EXTERN init_playerins_with_padmanip_return: qword
extern init_playerins_with_padmanip_helper: proc

PUBLIC init_playerins_with_padmanip_injection
init_playerins_with_padmanip_injection PROC
;original code
mov     [rsp+30h], ebp
mov     [rsp+20h], edx

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

lea     rcx, dword ptr [rsp+28h + 40h+8*7+28h] ;pass in ptr to the arg for what Manipulator type to use
call    init_playerins_with_padmanip_helper

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

jmp     init_playerins_with_padmanip_return
init_playerins_with_padmanip_injection ENDP


EXTERN MoveMapStep_SetPlayerLockOn_FromController_offset_return: qword
EXTERN ggpoStarted_ptr: qword

PUBLIC MoveMapStep_SetPlayerLockOn_FromController_offset_injection
MoveMapStep_SetPlayerLockOn_FromController_offset_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     exit
;original code
cmp     byte ptr [rcx+1430h], 0
setz    al
mov     [rcx+1431h], al
exit:
jmp     MoveMapStep_SetPlayerLockOn_FromController_offset_return
MoveMapStep_SetPlayerLockOn_FromController_offset_injection ENDP


EXTERN VirtualMultiDevice_GetInputI_return: qword
extern VirtualMultiDevice_GetInputI_helper: proc

PUBLIC VirtualMultiDevice_GetInputI_injection
VirtualMultiDevice_GetInputI_injection PROC
;original code
push    rbx
sub     rsp, 40h

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

call    VirtualMultiDevice_GetInputI_helper

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

cmp     al, 0
jne     continue
exit:
mov     byte ptr [rdx], 0
add     rsp, 40h
pop     rbx
ret

;original code
continue:
mov     rbx, rdx
xor     r8d, r8d
mov     edx, ecx
jmp     VirtualMultiDevice_GetInputI_return
VirtualMultiDevice_GetInputI_injection ENDP


EXTERN VirtualMultiDevice_GetStickInputI_return: qword
extern VirtualMultiDevice_GetStickInputI_helper: proc

PUBLIC VirtualMultiDevice_GetStickInputI_injection
VirtualMultiDevice_GetStickInputI_injection PROC
;original code
push    rbx
sub     rsp, 60h

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

call    VirtualMultiDevice_GetStickInputI_helper

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

cmp     al, 0
jne     continue
exit:
mov     byte ptr [rdx], 0
xorps   xmm0, xmm0
add     rsp, 60h
pop     rbx
ret

;original code
continue:
mov     rbx, rdx
movaps  xmmword ptr [rsp+50h], xmm6
jmp     VirtualMultiDevice_GetStickInputI_return
VirtualMultiDevice_GetStickInputI_injection ENDP


EXTERN PadMan_GetPadDevice_return: qword
extern PadMan_GetPadDevice_helper: proc

PUBLIC PadMan_GetPadDevice_injection
PadMan_GetPadDevice_injection PROC
;original code
mov     rcx, rbx
call    qword ptr [FUN_140391370]

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
push    rax
;push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

mov     rcx, rdi
call    PadMan_GetPadDevice_helper
mov     rcx, rax

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
;pop     rcx
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
call    qword ptr [PadMan_GetPadDevice]
jmp     PadMan_GetPadDevice_return
PadMan_GetPadDevice_injection ENDP

_TEXT    ENDS

END
