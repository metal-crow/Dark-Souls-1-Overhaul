_DATA SEGMENT

sub_140bd35b0   dq  140bd35b0h

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
call    qword ptr [sub_140bd35b0]

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

;force arg6 to be always 0 so the code doesn't ignore if the is_present flag is set
mov     r9b, 0

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


extern set_MemberFlags_bitflag_injection_helper: PROC
extern set_MemberFlags_bitflag_injection_return: qword

PUBLIC set_MemberFlags_bitflag_injection
set_MemberFlags_bitflag_injection PROC

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

mov     rcx, r8 ;the flag id
call    set_MemberFlags_bitflag_injection_helper

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

;check if we allow this flag to be set
cmp     al, 0
jne     exit
;just return from the function immediatly
add     rsp,028h
ret

;original code
exit:
mov     rax, r8
add     r9, 10h
shr     rax, 6
lea     rdx, [rcx+rax*8]
jmp     set_MemberFlags_bitflag_injection_return
set_MemberFlags_bitflag_injection ENDP

_TEXT    ENDS
END
