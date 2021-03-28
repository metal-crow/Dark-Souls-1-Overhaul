_DATA SEGMENT

_DATA ENDS

_TEXT    SEGMENT

EXTERN main_game_loop_injection_helper: PROC
EXTERN main_game_loop_injection_return: qword

PUBLIC main_game_loop_injection
main_game_loop_injection PROC

;original code
mov     rax, rsp
push    rdi
sub     rsp, 70h
mov     qword ptr [rax-58h], -2

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

call    main_game_loop_injection_helper

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

jmp main_game_loop_injection_return

main_game_loop_injection ENDP

_TEXT    ENDS

END
