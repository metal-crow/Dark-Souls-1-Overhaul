_DATA SEGMENT

sub_140426ba0   dq  140426ba0h

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


EXTERN homing_spell_trigger_injection_return: qword
EXTERN homing_spell_trigger_injection_helper_function: PROC

;Triggered when a homing spell (such as homing soul mass orb) is fired off
;Forcably send a type 1 packet out at this time, which we overload to include the relevant spell data
;We setup the data struct arg to the send packet function to our custom structure, which will be parsed opponent side by another injection

PUBLIC homing_spell_trigger_injection
homing_spell_trigger_injection PROC

cmp     dword ptr [rbx+9Ch], 10044000h ;check this homing bullet is from the PC, since we (the player with the bullet) are the sole decider of if it fires or not
jne     exit

FUNC_PROLOGUE
mov     ecx, dword ptr [R15+20h + 8] ;get the bullet target entity id
lea     rdx, dword ptr [rbx+10h] ;get the bullet position
call    homing_spell_trigger_injection_helper_function
FUNC_EPILOGUE

;original code
exit:
movaps  xmm0, [rsp+020h]
lea     rdx, [rsp+060h]
movaps  xmm1, [rsp+030h]

jmp     homing_spell_trigger_injection_return

homing_spell_trigger_injection ENDP


EXTERN type1_p2pPacket_parse_injection_return: qword
extern type1_p2pPacket_parse_injection_helper_function: PROC

;Check if the incoming type1 packet is a magic specific custom struct (created above)
;If so, don't do the normal code and instead extact it and save it to be later used
PUBLIC type1_p2pPacket_parse_injection
type1_p2pPacket_parse_injection PROC

cmp     dword ptr [rsp+30h], 7fc00001h
jne     normal_exit

lea     rcx, [rsp+30h] ;the incoming packet data
sub     rsp, 30h ;align the stack and add shadow stack space
call    type1_p2pPacket_parse_injection_helper_function

;exit the function we've injected into
add     rsp, 90h
pop     rdi
ret

;original code
normal_exit:
movups  xmm0, [rsp+30h]
movzx   eax,bpl
movups  xmm1, [rsp+40h]

jmp     type1_p2pPacket_parse_injection_return

type1_p2pPacket_parse_injection ENDP


EXTERN homing_spell_checkIfTriggered_injection_return: qword
extern homing_spell_checkIfTriggered_injection_helper_function: PROC

;Inject where the bullet is checked if it has a target
;And if the bullet params match a received network packet's details, set it's target
PUBLIC homing_spell_checkIfTriggered_injection
homing_spell_checkIfTriggered_injection PROC

;original code
movaps  xmm2, xmm6
lea     rdx, [rsp+20h]
mov     rcx, rbx
call    qword ptr [sub_140426ba0]

FUNC_PROLOGUE
mov     rcx, rbx ;bulletIns
lea     rdx, dword ptr [R15+20h + 8]; bulletParamEntry.target_id
call    homing_spell_checkIfTriggered_injection_helper_function
FUNC_EPILOGUE

jmp     homing_spell_checkIfTriggered_injection_return

homing_spell_checkIfTriggered_injection ENDP


_TEXT    ENDS

END
