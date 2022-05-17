_DATA SEGMENT

qword_141D1A370 dq  141D1A370

_DATA ENDS

_TEXT    SEGMENT

EXTERN sendType1NetMessage_return: qword
extern sendType1NetMessage_helper: proc

PUBLIC sendType1NetMessage_injection
sendType1NetMessage_injection PROC
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
call    sendType1NetMessage_helper
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

;check if we return or not
test    eax, eax
jz      normal
ret

;original code
normal:
push    rbx
sub     rsp, 20h
mov     rbx, rcx
mov     rcx, cs:qword_141D1A370
jmp     sendType1NetMessage_return
sendType1NetMessage_injection ENDP

EXTERN readparseType1NetMessage_return: qword
extern readparseType1NetMessage_helper: proc

PUBLIC readparseType1NetMessage_injection
readparseType1NetMessage_injection PROC
;original code
mov     [rsp+10h], rbx
mov     [rsp+18h], rsi

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
call    readparseType1NetMessage_helper
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

;check if we return or not
test    eax, eax
jz      normal
ret

;original code
normal:
push    rdi
sub     rsp, 60h
jmp     readparseType1NetMessage_return
readparseType1NetMessage_injection ENDP

EXTERN send_generalplayerinfo_return: qword
extern send_generalplayerinfo_helper: proc

PUBLIC send_generalplayerinfo_injection
send_generalplayerinfo_injection PROC
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
call    send_generalplayerinfo_helper
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
mov     r11, rsp
push    rbp
push    rsi
lea     rbp, [r11-5Fh]
sub     rsp, 0B8h
jmp     send_generalplayerinfo_return
send_generalplayerinfo_injection ENDP

EXTERN Read_GeneralPlayerData_return: qword
extern Read_GeneralPlayerData_helper: proc

PUBLIC Read_GeneralPlayerData_injection
Read_GeneralPlayerData_injection PROC
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
call    Read_GeneralPlayerData_helper
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
mov     r11, rsp
push    rbp
push    rdi
push    r15
lea     rbp, [r11-5Fh]
sub     rsp, 0D0h
jmp     Read_GeneralPlayerData_return
Read_GeneralPlayerData_injection ENDP

EXTERN sendType10NetMessage_return: qword
extern sendType10NetMessage_helper: proc

PUBLIC sendType10NetMessage_injection
sendType10NetMessage_injection PROC
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
call    sendType10NetMessage_helper
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

;check if we return or not
test    eax, eax
jz      normal
ret

;original code
normal:
push    rdi
sub     rsp, 0C0h
mov     qword ptr [rsp+30h], 0FFFFFFFFFFFFFFFEh
jmp     sendType10NetMessage_return
sendType10NetMessage_injection ENDP

EXTERN sendType10NetMessage2_return: qword
extern sendType10NetMessage2_helper: proc

PUBLIC sendType10NetMessage2_injection
sendType10NetMessage2_injection PROC
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
call    sendType10NetMessage2_helper
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

;check if we return or not
test    eax, eax
jz      normal
ret

;original code
normal:
push    rdi
sub     rsp, 0B0h
mov     qword ptr [rsp+30h], 0FFFFFFFFFFFFFFFEh
jmp     sendType10NetMessage2_return
sendType10NetMessage2_injection ENDP

EXTERN readparseType10NetMessage_return: qword
extern readparseType10NetMessage_helper: proc

PUBLIC readparseType10NetMessage_injection
readparseType10NetMessage_injection PROC
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
call    readparseType10NetMessage_helper
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

;check if we return or not
test    eax, eax
jz      normal
ret

;original code
normal:
mov     rax, rsp
push    rdi
sub     rsp, 80h
mov     qword ptr [rax-58h], 0FFFFFFFFFFFFFFFEh
jmp     readparseType10NetMessage_return
readparseType10NetMessage_injection ENDP

_TEXT    ENDS

END
