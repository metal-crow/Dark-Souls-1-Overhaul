_TEXT    SEGMENT

extern NameCrash_prevent_return: qword
extern NameCrash_prevent_helper: proc

PUBLIC NameCrash_prevent
NameCrash_prevent PROC

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

lea     rcx, [rsi+98h] ;ptr to loaded name string
call    NameCrash_prevent_helper

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
mov     rbx, [rsp+30h]
mov     rsi, [rsp+38h]
add     rsp, 20h

jmp     NameCrash_prevent_return

NameCrash_prevent ENDP

_TEXT    ENDS

END
