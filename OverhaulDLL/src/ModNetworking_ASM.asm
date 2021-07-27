_DATA SEGMENT

_DATA ENDS

_TEXT    SEGMENT


EXTERN AcceptP2PSessionWithUser_injection_helper: PROC
EXTERN AcceptP2PSessionWithUser_injection_return: qword

PUBLIC AcceptP2PSessionWithUser_injection
AcceptP2PSessionWithUser_injection PROC

;original code
mov     qword ptr [rsp+20h], -2
mov     [rsp+50h], rbx

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

mov     rcx, qword ptr [rdx] ;steamid64
call    AcceptP2PSessionWithUser_injection_helper

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

;if we should decline this connection, abort function
cmp     al, 0
je      abort
jmp     AcceptP2PSessionWithUser_injection_return

abort:
add     rsp, 40h
pop     rdi
ret
AcceptP2PSessionWithUser_injection ENDP


EXTERN IsP2PPacketAvailable_Replacement_injection_helper: PROC

EXTERN IsP2PPacketAvailable_1_Replacement_injection_return: qword
PUBLIC IsP2PPacketAvailable_1_Replacement_injection
IsP2PPacketAvailable_1_Replacement_injection PROC

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

lea     rcx, [RSP + 20h + 40h+8*6+20h] ;pcubMsgSize
mov     rdx, 0 ;channel 0
call    IsP2PPacketAvailable_Replacement_injection_helper

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

jmp     IsP2PPacketAvailable_1_Replacement_injection_return
IsP2PPacketAvailable_1_Replacement_injection ENDP

EXTERN IsP2PPacketAvailable_2_Replacement_injection_return: qword
PUBLIC IsP2PPacketAvailable_2_Replacement_injection
IsP2PPacketAvailable_2_Replacement_injection PROC

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

lea     rcx, [RSP + 20h + 40h+8*6+20h] ;pcubMsgSize
mov     rdx, 0 ;channel 0
call    IsP2PPacketAvailable_Replacement_injection_helper

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

jmp     IsP2PPacketAvailable_2_Replacement_injection_return
IsP2PPacketAvailable_2_Replacement_injection ENDP


EXTERN ReadP2PPacket_Replacement_injection_helper: PROC
EXTERN ReadP2PPacket_Replacement_injection_return: qword

PUBLIC ReadP2PPacket_Replacement_injection
ReadP2PPacket_Replacement_injection PROC

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
xor     r12, r12
push    r12 ;nChannel = 0
push    r12 ;extra for stack alignment
sub     rsp, 20h

mov     rcx, r15 ;pubDest
mov     edx, ebx ;cubDest
lea     r8, [rbp-38h] ;pcubMsgSize
lea     r9, [rbp-40h] ;psteamIDRemote
call    ReadP2PPacket_Replacement_injection_helper

add     rsp, 30h
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

jmp     ReadP2PPacket_Replacement_injection_return
ReadP2PPacket_Replacement_injection ENDP


_TEXT    ENDS

END
