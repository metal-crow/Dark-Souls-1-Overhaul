_DATA SEGMENT

LAB_141078a2e dq  141078A2Eh

_DATA ENDS

_TEXT    SEGMENT

EXTERN rce1_injection_return: qword
extern rce1_helper: proc

PUBLIC rce1_injection
rce1_injection PROC
;original code
call    qword ptr [rax+88h]
mov     ecx, ebx
mov     [rbp+rcx*2-50h], ax ;write utf16 char

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

;ecx is the offset in the string we're writing to
lea     rdx, [rbp-50h] ;pointer to the base of the string
call    rce1_helper
mov     ebx, eax ;use the return value to set the offset

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

;exit the loop if the value is too high
cmp     ebx, 1022
jle     exit
jmp     qword ptr [LAB_141078a2e]
;original code
exit:
inc     ebx
jmp     rce1_injection_return
rce1_injection ENDP

_TEXT    ENDS

END
