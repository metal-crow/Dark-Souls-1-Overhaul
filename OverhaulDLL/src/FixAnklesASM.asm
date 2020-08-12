_TEXT    SEGMENT


EXTERN main_fix_ankles_helper: PROC
EXTERN main_fix_ankles_injection_return: qword

PUBLIC main_fix_ankles_injection
main_fix_ankles_injection PROC

;original code
lea     rbp, [rsp-188h]
sub     rsp, 288h

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

;passing in the parentfootIk ptr implicitly
call    main_fix_ankles_helper

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
jmp     main_fix_ankles_injection_return

main_fix_ankles_injection ENDP

_TEXT    ENDS

END
