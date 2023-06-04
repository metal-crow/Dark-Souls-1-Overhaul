_DATA SEGMENT

sub_140bd35b0   dq  140bd35b0h

_DATA ENDS

_TEXT SEGMENT

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

FUNC_EPILOGUE macro
	mov		r15, [rsp + 20h]
	mov		r11, [rsp + 28h]
	mov		r10, [rsp + 30h]
	mov		r9, [rsp + 38h]
	mov		r8, [rsp + 40h]
	mov		rdx, [rsp + 48h]
	mov		rcx, [rsp + 50h]
	mov		rax, [rsp + 58h]
	movaps	xmm5, [rsp + 60h]
	movaps	xmm4, [rsp + 70h]
	movaps	xmm3, [rsp + 80h]
	movaps	xmm2, [rsp + 90h]
	movaps	xmm1, [rsp + 0A0h]
	movaps	xmm0, [rsp + 0B0h]
	mov		rsp, r15
	pop		r15
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

FUNC_EPILOGUE_NOR9 macro
	mov		r15, [rsp + 20h]
	mov		r11, [rsp + 28h]
	mov		r10, [rsp + 30h]
	mov		r8, [rsp + 40h]
	mov		rdx, [rsp + 48h]
	mov		rcx, [rsp + 50h]
	mov		rax, [rsp + 58h]
	movaps	xmm5, [rsp + 60h]
	movaps	xmm4, [rsp + 70h]
	movaps	xmm3, [rsp + 80h]
	movaps	xmm2, [rsp + 90h]
	movaps	xmm1, [rsp + 0A0h]
	movaps	xmm0, [rsp + 0B0h]
	mov		rsp, r15
	pop		r15
endm

extern game_write_playerdata_to_flatbuffer_injection_helper: PROC
extern game_write_playerdata_to_flatbuffer_injection_return: qword

PUBLIC game_write_playerdata_to_flatbuffer_injection
game_write_playerdata_to_flatbuffer_injection PROC

FUNC_PROLOGUE
mov     rdx, r9 ;array length
mov     rcx, r8 ;array start
call    game_write_playerdata_to_flatbuffer_injection_helper
mov     r9, rax ;save over the length
FUNC_EPILOGUE_NOR9

;original code
lea     rdx, [rsp+24h]
mov     rcx, rbx
call    qword ptr [sub_140bd35b0]

jmp     game_write_playerdata_to_flatbuffer_injection_return

game_write_playerdata_to_flatbuffer_injection ENDP


extern construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper: PROC
extern construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return: qword

PUBLIC construct_flatbuffer_from_PlayerStatus_MemberFlags_injection
construct_flatbuffer_from_PlayerStatus_MemberFlags_injection PROC

;original code
lea     rbp, [rsp-110h]
sub     rsp, 210h

FUNC_PROLOGUE
call    construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper
FUNC_EPILOGUE

;force arg6 to be always 0 so the code doesn't ignore if the is_present flag is set
mov     r9b, 0

jmp     construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return
construct_flatbuffer_from_PlayerStatus_MemberFlags_injection ENDP


extern finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper: PROC
extern finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return: qword

PUBLIC finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection
finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection PROC

FUNC_PROLOGUE
call    finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper
FUNC_EPILOGUE

;original code
mov     rbx, [rsp+268h]
add     rsp, 210h

jmp     finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return
finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection ENDP


extern set_MemberFlags_bitflag_injection_helper: PROC
extern set_MemberFlags_bitflag_injection_return: qword

PUBLIC set_MemberFlags_bitflag_injection
set_MemberFlags_bitflag_injection PROC

FUNC_PROLOGUE
mov     rcx, r8 ;the flag id
call    set_MemberFlags_bitflag_injection_helper
FUNC_EPILOGUE_NORAX

;check if we allow this flag to be set
cmp     al, 0
jne     exit
;just return from the function immediatly
add     rsp,028h
ret

;original code
exit:
mov     rax, r8
add     r9, 10h
shr     rax, 6
lea     rdx, [rcx+rax*8]
jmp     set_MemberFlags_bitflag_injection_return
set_MemberFlags_bitflag_injection ENDP

_TEXT    ENDS
END
