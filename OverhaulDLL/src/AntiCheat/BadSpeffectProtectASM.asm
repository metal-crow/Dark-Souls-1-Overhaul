_DATA SEGMENT

sub_140509560   dq  140509560h

_DATA ENDS

_TEXT    SEGMENT

extern ReadParseType18_packet_return: qword
extern ReadParseType18_packet_injection_helper: proc

PUBLIC ReadParseType18_packet_injection
ReadParseType18_packet_injection PROC

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

mov     rcx, rsi ;pointer to the packet data
call    ReadParseType18_packet_injection_helper

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
mov     rcx, 141D151B0h
mov     rcx, qword ptr [rcx]
test    rcx, rcx
jz      abort_exit
jmp     normal_exit
abort_exit:
mov     r14, 14050687Eh
jmp     r14
normal_exit:
mov     r14, 1427EF93Ch
jmp     r14

ReadParseType18_packet_injection ENDP


extern ReadParseType35_packet_return: qword
extern ReadParseType35_packet_injection_helper: proc

PUBLIC ReadParseType35_packet_injection
ReadParseType35_packet_injection PROC

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

mov     rcx, rsi ;pointer to the packet data
call    ReadParseType35_packet_injection_helper

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
test    rcx, rcx
push    rbp
mov     rbp, 142898105h

jmp     ReadParseType35_packet_return

ReadParseType35_packet_injection ENDP

_TEXT    ENDS

END
