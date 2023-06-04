_DATA SEGMENT

_DATA ENDS

_TEXT    SEGMENT

FUNC_PROLOGUE macro
	push	r15
	mov		r15, rsp
	and		rsp, -10h
	sub		rsp, 0C0h
	movaps	[rsp + 0B0h], xmm0
	movaps	[rsp + 0A0h], xmm1
	movaps	[rsp + 90h], xmm2
	movaps	[rsp + 80h], xmm3
	movaps	[rsp + 70h], xmm4
	movaps	[rsp + 60h], xmm5
	mov		[rsp + 58h], rax
	mov		[rsp + 50h], rcx
	mov		[rsp + 48h], rdx
	mov		[rsp + 40h], r8
	mov		[rsp + 38h], r9
	mov		[rsp + 30h], r10
	mov		[rsp + 28h], r11
	mov		[rsp + 20h], r15
endm

FUNC_PROLOGUE_PLUS10_USER13 macro
	push	r13
	mov		r13, rsp
	and		rsp, -10h
	sub		rsp, 0D0h
	movaps	[rsp + 0C0h], xmm0
	movaps	[rsp + 0B0h], xmm1
	movaps	[rsp + 0A0h], xmm2
	movaps	[rsp + 90h], xmm3
	movaps	[rsp + 80h], xmm4
	movaps	[rsp + 70h], xmm5
	mov		[rsp + 68h], rax
	mov		[rsp + 60h], rcx
	mov		[rsp + 58h], rdx
	mov		[rsp + 50h], r8
	mov		[rsp + 48h], r9
	mov		[rsp + 40h], r10
	mov		[rsp + 38h], r11
	mov		[rsp + 30h], r13
endm

FUNC_EPILOGUE_NORAX macro
	mov		r15, [rsp + 20h]
	mov		r11, [rsp + 28h]
	mov		r10, [rsp + 30h]
	mov		r9, [rsp + 38h]
	mov		r8, [rsp + 40h]
	mov		rdx, [rsp + 48h]
	mov		rcx, [rsp + 50h]
	movaps	xmm5, [rsp + 60h]
	movaps	xmm4, [rsp + 70h]
	movaps	xmm3, [rsp + 80h]
	movaps	xmm2, [rsp + 90h]
	movaps	xmm1, [rsp + 0A0h]
	movaps	xmm0, [rsp + 0B0h]
	mov		rsp, r15
	pop		r15
endm

FUNC_EPILOGUE_NORAX_PLUS10_USER13 macro
	mov		r13, [rsp + 30h]
	mov		r11, [rsp + 38h]
	mov		r10, [rsp + 40h]
	mov		r9, [rsp + 48h]
	mov		r8, [rsp + 50h]
	mov		rdx, [rsp + 58h]
	mov		rcx, [rsp + 60h]
	movaps	xmm5, [rsp + 70h]
	movaps	xmm4, [rsp + 80h]
	movaps	xmm3, [rsp + 90h]
	movaps	xmm2, [rsp + 0A0h]
	movaps	xmm1, [rsp + 0B0h]
	movaps	xmm0, [rsp + 0C0h]
	mov		rsp, r13
	pop		r13
endm

EXTERN AcceptP2PSessionWithUser_injection_helper: PROC
EXTERN AcceptP2PSessionWithUser_injection_return: qword

PUBLIC AcceptP2PSessionWithUser_injection
AcceptP2PSessionWithUser_injection PROC

;original code
mov     qword ptr [rsp+20h], -2
mov     [rsp+50h], rbx

FUNC_PROLOGUE
mov     rcx, qword ptr [rdx] ;steamid64
call    AcceptP2PSessionWithUser_injection_helper
FUNC_EPILOGUE_NORAX

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

FUNC_PROLOGUE
lea     rcx, [R15+20h + 8] ;pcubMsgSize
mov     rdx, 0 ;channel 0
call    IsP2PPacketAvailable_Replacement_injection_helper
FUNC_EPILOGUE_NORAX

jmp     IsP2PPacketAvailable_1_Replacement_injection_return

IsP2PPacketAvailable_1_Replacement_injection ENDP

EXTERN IsP2PPacketAvailable_2_Replacement_injection_return: qword
PUBLIC IsP2PPacketAvailable_2_Replacement_injection
IsP2PPacketAvailable_2_Replacement_injection PROC

FUNC_PROLOGUE
lea     rcx, [R15+20h + 8] ;pcubMsgSize
mov     rdx, 0 ;channel 0
call    IsP2PPacketAvailable_Replacement_injection_helper
FUNC_EPILOGUE_NORAX

jmp     IsP2PPacketAvailable_2_Replacement_injection_return

IsP2PPacketAvailable_2_Replacement_injection ENDP


EXTERN ReadP2PPacket_Replacement_injection_helper: PROC
EXTERN ReadP2PPacket_Replacement_injection_return: qword

PUBLIC ReadP2PPacket_Replacement_injection
ReadP2PPacket_Replacement_injection PROC

FUNC_PROLOGUE_PLUS10_USER13
mov     qword ptr [rsp + 20h], 0 ;nChannel = 0
mov     rcx, r15 ;pubDest
mov     edx, ebx ;cubDest
lea     r8, [rbp-38h] ;pcubMsgSize (DSR passes null)
lea     r9, [rbp-40h] ;psteamIDRemote (DSR passes null)
call    ReadP2PPacket_Replacement_injection_helper
FUNC_EPILOGUE_NORAX_PLUS10_USER13

jmp     ReadP2PPacket_Replacement_injection_return

ReadP2PPacket_Replacement_injection ENDP


EXTERN SendP2PPacket_Replacement_injection_helper: PROC

EXTERN SendP2PPacket_voice_Replacement_injection_return: qword
PUBLIC SendP2PPacket_voice_Replacement_injection
SendP2PPacket_voice_Replacement_injection PROC

FUNC_PROLOGUE_PLUS10_USER13
mov     qword ptr [rsp + 20h], 0 ;nChannel = 0
mov     rcx, [rbx] ;steamIDRemote
lea     rdx, [R13+70h + 8] ;pubData
mov     r8d, [R13+50h + 8] ;cubData
inc     r8d ;i dunno, DSR does it
mov     r9d, 1 ;eP2PSendType::k_EP2PSendUnreliableNoDelay (voice specific)
call    SendP2PPacket_Replacement_injection_helper
FUNC_EPILOGUE_NORAX_PLUS10_USER13

jmp     SendP2PPacket_voice_Replacement_injection_return

SendP2PPacket_voice_Replacement_injection ENDP

EXTERN SendP2PPacket_Replacement_injection_return: qword
PUBLIC SendP2PPacket_Replacement_injection
SendP2PPacket_Replacement_injection PROC

FUNC_PROLOGUE_PLUS10_USER13
mov     qword ptr [rsp + 20h], 0 ;nChannel = 0
mov     rcx, [rbp+0C8h] ;steamIDRemote
mov     rdx, r14 ;pubData
mov     r8d, r15d ;cubData
mov     r9d, [R13+40h + 8] ;eP2PSendType
call    SendP2PPacket_Replacement_injection_helper
FUNC_EPILOGUE_NORAX_PLUS10_USER13

jmp     SendP2PPacket_Replacement_injection_return

SendP2PPacket_Replacement_injection ENDP


EXTERN CloseP2PSessionWithUser_Replacement_injection_helper: PROC
EXTERN CloseP2PSessionWithUser_Replacement_injection_return: qword

PUBLIC CloseP2PSessionWithUser_Replacement_injection
CloseP2PSessionWithUser_Replacement_injection PROC

FUNC_PROLOGUE
mov     rcx, [rbx+0C8h]
call    CloseP2PSessionWithUser_Replacement_injection_helper
FUNC_EPILOGUE_NORAX

jmp     CloseP2PSessionWithUser_Replacement_injection_return

CloseP2PSessionWithUser_Replacement_injection ENDP

_TEXT    ENDS

END
