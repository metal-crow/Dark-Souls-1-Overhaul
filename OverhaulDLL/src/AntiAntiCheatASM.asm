_DATA SEGMENT

sub_140BCEDB0   dq  140BCEDB0h
sub_14112C930   dq  14112C930h

_DATA ENDS

_TEXT SEGMENT

extern game_write_playerdata_to_flatbuffer_injection_helper: PROC
extern game_write_playerdata_to_flatbuffer_injection_return: qword

PUBLIC game_write_playerdata_to_flatbuffer_injection
game_write_playerdata_to_flatbuffer_injection PROC

;save temp registers
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
;push    r9
push    r10
push    r11
sub     rsp, 20h

mov     rdx, r9 ;array length
mov     rcx, r8 ;array start
call    game_write_playerdata_to_flatbuffer_injection_helper
mov     r9, rax ;save over the length

add     rsp, 20h
pop     r11
pop     r10
;pop     r9
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
lea     rdx, [rsp+24h]
mov     rcx, rbx
call    qword ptr [sub_140BCEDB0]

jmp     game_write_playerdata_to_flatbuffer_injection_return

game_write_playerdata_to_flatbuffer_injection ENDP


extern construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper: PROC
extern construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return: qword

PUBLIC construct_flatbuffer_from_PlayerStatus_MemberFlags_injection
construct_flatbuffer_from_PlayerStatus_MemberFlags_injection PROC

;original code
lea     rbp, [rsp-110h]
sub     rsp, 210h

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

call    construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper

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

jmp     construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return
construct_flatbuffer_from_PlayerStatus_MemberFlags_injection ENDP


extern finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper: PROC
extern finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return: qword

PUBLIC finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection
finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection PROC

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

call    finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper

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
mov     rbx, [rsp+268h]
add     rsp, 210h

jmp     finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return
finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection ENDP


extern prevent_send_server_requests_injection_return: qword

PUBLIC prevent_send_server_requests_injection
prevent_send_server_requests_injection PROC
movzx   eax, cl

cmp     eax, 46
jg      abort
;compare eax(type) to allowed list
cmp     eax, 33
je      normal
cmp     eax, 6
je      normal
jmp     abort

;original code
normal:
mov     rcx, 141B0F400h
mov     rax, [rcx+rax*8]
jmp     prevent_send_server_requests_injection_return

abort:
add     rsp, 0F0h
pop     r15
pop     r14
pop     r13
pop     r12
pop     rdi
pop     rsi
pop     rbp
ret
prevent_send_server_requests_injection ENDP

extern prevent_send_server_pushs_injection_return: qword

PUBLIC prevent_send_server_pushs_injection
prevent_send_server_pushs_injection PROC
;original code
mov     rdx, r12
mov     rcx, r15
call    qword ptr [sub_14112C930]

;get the packet type
push    rbx
;offset_loc = data + 4 + -Dword(data)
mov     rdi, rax
add     rdi, 4h
mov     rbx, 0
mov     ebx, dword ptr [rax]
sub     rdi, rbx
;offset = Word(offset_loc)
mov     rbx, 0
mov     bx, word ptr [rdi]
;type = Byte(offset+data)
mov     rdi, rbx
add     rdi, rax
mov     dil, byte ptr [rdi]

pop     rbx

cmp     dil, 51
jg      abort
;compare dil(type) to allowed list
jmp     abort
mov     rdi, rax
jmp     prevent_send_server_pushs_injection_return

abort:
mov     rax, 0
mov     rdi, rax
prevent_send_server_pushs_injection ENDP

_TEXT    ENDS
END
