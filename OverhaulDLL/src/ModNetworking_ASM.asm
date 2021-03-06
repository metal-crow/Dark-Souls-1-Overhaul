_DATA SEGMENT

getNetMessage_savedDataPtr  dq  0
sub_140514EF0   dq 140514EF0h
sub_141078C90   dq 141078C90h

_DATA ENDS

_TEXT    SEGMENT

EXTERN getNetMessage_injection_helper: PROC
EXTERN getNetMessage_injection_return: qword

PUBLIC getNetMessage_injection
getNetMessage_injection PROC

;original code (so we call recieve before we parse the data)
mov     r9d, [rsp+50h]
mov     r8, r14
mov     qword ptr [getNetMessage_savedDataPtr], r8 ;save this data ptr since we lose it otherwise
mov     edx, ebx
mov     rcx, r10
call    qword ptr [sub_140514EF0]

sub     rsp, 8h ;stack align
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
sub     rsp, 20h

mov     r8d, ebx
mov     edx, eax
mov     rcx, qword ptr [getNetMessage_savedDataPtr]
call    getNetMessage_injection_helper

add     rsp, 20h
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
add     rsp, 8h

jmp     getNetMessage_injection_return

getNetMessage_injection ENDP


EXTERN GetSteamData_Packet_injection_helper: PROC
EXTERN GetSteamData_Packet_injection_return: qword

PUBLIC GetSteamData_Packet_injection
GetSteamData_Packet_injection PROC

;original code (get the type byte before we parse the data)
lea     rcx, [rsp+28h]
call    qword ptr [sub_141078C90]

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
;push    rax ;used as return
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

mov     r8, rdi ;SteamSessionMemberLight
mov     edx, eax ;the packet type
lea     rcx, [rsp + 28h + 40h + 8*6 + 20h] ;the data struct
call    GetSteamData_Packet_injection_helper

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

;original code
add     eax, 0FFFFFFFCh
cmp     eax, 2Eh

jmp     GetSteamData_Packet_injection_return

GetSteamData_Packet_injection ENDP


EXTERN sendPacket_injection_helper: PROC
EXTERN sendPacket_injection_return: qword

PUBLIC sendPacket_injection
sendPacket_injection PROC

;original code
sub     rsp, 80h
mov     qword ptr [rax-78h], 0FFFFFFFFFFFFFFFEh

sub     rsp, 8h ;stack align
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
sub     rsp, 20h

mov     edx, dword ptr [rsp+0D0h+40h+8*8+20h]
mov     rcx, r9
call    sendPacket_injection_helper

add     rsp, 20h
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
add     rsp, 8h

jmp     sendPacket_injection_return

sendPacket_injection ENDP


EXTERN SendRawP2PPacket_injection_helper: PROC
EXTERN SendRawP2PPacket_injection_return: qword

PUBLIC SendRawP2PPacket_injection
SendRawP2PPacket_injection PROC

;original code
xor     rax, rsp
mov     [rsp+510h], rax
mov     rbx, r8

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
;push    rax ;don't save, use as return
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

mov     rcx, r8 ;data
mov     rdx, r9 ;data_len
mov     r8d, dword ptr [rsp+590h+40h+8*6+20h] ;type
call    SendRawP2PPacket_injection_helper

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

mov     r9, rax ;use return as data_len

jmp     SendRawP2PPacket_injection_return

SendRawP2PPacket_injection ENDP


EXTERN ParseRawP2PPacketType_injection_helper: PROC
EXTERN ParseRawP2PPacketType_injection_return: qword

PUBLIC ParseRawP2PPacketType_injection
ParseRawP2PPacketType_injection PROC

;original code
movzx   eax, byte ptr [r15] ;gets the encapsulated packet type

;check if this is our custom packet type
cmp     eax, 20 ;(4 | (1 << 4)
jne     exit

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

mov     rcx, r15 ;start of the packet data
mov     rdx, qword ptr [rbp-40h] ;sender's steamId
call    ParseRawP2PPacketType_injection_helper

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

exit:
;original code
dec     r14d
mov     [rsp+0C8h], rsi

jmp     ParseRawP2PPacketType_injection_return

ParseRawP2PPacketType_injection ENDP


EXTERN type1_32byte_p2pPacket_parse_rollback_injection_helper: PROC
EXTERN type1_32byte_p2pPacket_parse_rollback_injection_return: qword

PUBLIC type1_32byte_p2pPacket_parse_rollback_injection
type1_32byte_p2pPacket_parse_rollback_injection PROC

;original code will be handled in the helper

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

call    type1_32byte_p2pPacket_parse_rollback_injection_helper

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

;original code
test    eax, eax

jmp     type1_32byte_p2pPacket_parse_rollback_injection_return

type1_32byte_p2pPacket_parse_rollback_injection ENDP

EXTERN type1_40byte_p2pPacket_parse_rollback_injection_helper: PROC
EXTERN type1_40byte_p2pPacket_parse_rollback_injection_return: qword

PUBLIC type1_40byte_p2pPacket_parse_rollback_injection
type1_40byte_p2pPacket_parse_rollback_injection PROC

;original code
lea     rdx, [rbx+30h]
;rest of original code will be handled in the helper

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

call    type1_40byte_p2pPacket_parse_rollback_injection_helper

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

jmp     type1_40byte_p2pPacket_parse_rollback_injection_return

type1_40byte_p2pPacket_parse_rollback_injection ENDP


EXTERN type1_p2pPacket_send_rollback_injection_helper: PROC
EXTERN type1_p2pPacket_sending_rollback_injection_return: qword

PUBLIC type1_p2pPacket_sending_rollback_injection
type1_p2pPacket_sending_rollback_injection PROC

;original code
lea     r9d, [rdx+27h]
jnz     size40
lea     r9d, [rdx+1fh]
size40:

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

call    type1_p2pPacket_send_rollback_injection_helper

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

jmp     type1_p2pPacket_sending_rollback_injection_return

type1_p2pPacket_sending_rollback_injection ENDP

_TEXT    ENDS

END
