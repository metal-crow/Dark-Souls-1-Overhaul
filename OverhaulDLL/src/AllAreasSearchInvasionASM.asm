_DATA SEGMENT

_DATA ENDS

_TEXT    SEGMENT


EXTERN Send_Type17_GeneralRequestTask_injection_helper: PROC
EXTERN Send_Type17_GeneralRequestTask_injection_return: qword

PUBLIC Send_Type17_GeneralRequestTask_injection
Send_Type17_GeneralRequestTask_injection PROC

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

mov     rcx, r8
call    Send_Type17_GeneralRequestTask_injection_helper

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
sub     rsp, 0D0h
mov     qword ptr [rbp-49h], -2
jmp     Send_Type17_GeneralRequestTask_injection_return
Send_Type17_GeneralRequestTask_injection ENDP


_TEXT    ENDS

END
