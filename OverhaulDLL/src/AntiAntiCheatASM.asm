_DATA SEGMENT

sub_140BCFEC0   dq  140BCFEC0h

_DATA ENDS

_TEXT SEGMENT

extern game_send_playerdata_to_server_helper: PROC
extern game_send_playerdata_to_server_injection_return: qword

PUBLIC game_send_playerdata_to_server_injection
game_send_playerdata_to_server_injection PROC

;save temp registers
sub     rsp, 8 ;stack not 16 bit aligned?!?!?!?
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

mov     rcx, rdi
call    game_send_playerdata_to_server_helper

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
add     rsp, 8

;original code
mov     r9d, eax
mov     r8d, r12d
mov     rdx, rbx
lea     rcx, [rbp+52h]
call    qword ptr [sub_140BCFEC0]
jmp     game_send_playerdata_to_server_injection_return

game_send_playerdata_to_server_injection ENDP

_TEXT    ENDS

END
