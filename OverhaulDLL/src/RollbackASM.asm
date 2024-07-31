_DATA SEGMENT

sub_1401862A0   dq  1401862A0h
nullsub_78     dq  140185470h
Build_BulletIns_FollowupBullet  dq  140fdcac0h
_DATA ENDS

_TEXT    SEGMENT

FUNC_PROLOGUE macro
    pushfq 
    push    rax
    mov     rax,rsp
    and     rsp,-10h
    sub     rsp,000002A0h
    fxsave  [rsp+20h]
    mov     [rsp+00000220h],rbx
    mov     [rsp+00000228h],rcx
    mov     [rsp+00000230h],rdx
    mov     [rsp+00000238h],rsi
    mov     [rsp+00000240h],rdi
    mov     [rsp+00000248h],rax
    mov     [rsp+00000250h],rbp
    mov     [rsp+00000258h],r8
    mov     [rsp+00000260h],r9
    mov     [rsp+00000268h],r10
    mov     [rsp+00000270h],r11
    mov     [rsp+00000278h],r12
    mov     [rsp+00000280h],r13
    mov     [rsp+00000288h],r14
    mov     [rsp+00000290h],r15
endm

FUNC_EPILOGUE macro
    mov     r15,[rsp+00000290h]
    mov     r14,[rsp+00000288h]
    mov     r13,[rsp+00000280h]
    mov     r12,[rsp+00000278h]
    mov     r11,[rsp+00000270h]
    mov     r10,[rsp+00000268h]
    mov     r9, [rsp+00000260h]
    mov     r8, [rsp+00000258h]
    mov     rbp,[rsp+00000250h]
    mov     rdi,[rsp+00000240h]
    mov     rsi,[rsp+00000238h]
    mov     rdx,[rsp+00000230h]
    mov     rcx,[rsp+00000228h]
    mov     rbx,[rsp+00000220h]
    fxrstor [rsp+20h]
    mov     rsp,[rsp+00000248h]
    pop     rax
    popfq 
endm

FUNC_PROLOGUE_LITE macro
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

FUNC_EPILOGUE_LITE macro
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

FUNC_EPILOGUE_LITE_NORAX macro
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

EXTERN sendNetMessage_return: qword
extern sendNetMessage_helper: proc

PUBLIC sendNetMessage_injection
sendNetMessage_injection PROC

FUNC_PROLOGUE_LITE
call    sendNetMessage_helper
FUNC_EPILOGUE_LITE_NORAX
;check if we abort this call or not
test    al, al
jnz     normal
xor     al, al ;aborting call, so return false
ret

normal:
;original code
mov     rax, rsp
push    rdi
push    r12
push    r13
push    r14
push    r15
sub     rsp, 80h
mov     qword ptr [rax-78h], -2
mov     [rax+10h], rbx
mov     [rax+18h], rbp
mov     [rax+20h], rsi
jmp     sendNetMessage_return
sendNetMessage_injection ENDP


EXTERN getNetMessage_return: qword
extern getNetMessage_helper: proc

PUBLIC getNetMessage_injection
getNetMessage_injection PROC

FUNC_PROLOGUE_LITE
call    getNetMessage_helper
FUNC_EPILOGUE_LITE_NORAX
;check if we abort this call or not
test    al, al
jnz     normal
xor     eax, eax ;aborting call, so return 0 bytes
ret

normal:
;original code
mov     [rsp+8], rbx
mov     [rsp+10h], rbp
mov     [rsp+18h], rsi
mov     [rsp+20h], rdi
push    r14
sub     rsp, 20h
jmp     getNetMessage_return
getNetMessage_injection ENDP


EXTERN dsr_frame_finished_return: qword
extern dsr_frame_finished_helper: proc

PUBLIC dsr_frame_finished_injection
dsr_frame_finished_injection PROC
;original code
movzx   ecx, dil
xor     edx, edx
test    al, al
cmovnz  ecx, edx
movzx   eax, cl

FUNC_PROLOGUE
call    dsr_frame_finished_helper
FUNC_EPILOGUE

jmp     dsr_frame_finished_return
dsr_frame_finished_injection ENDP


EXTERN init_playerins_with_padmanip_return: qword
extern init_playerins_with_padmanip_helper: proc

PUBLIC init_playerins_with_padmanip_injection
init_playerins_with_padmanip_injection PROC
;original code
mov     [rsp+30h], ebp
mov     [rsp+20h], edx

FUNC_PROLOGUE
lea     rcx, dword ptr [RAX+28h + 10h] ;pass in ptr to the arg for what Manipulator type to use
call    init_playerins_with_padmanip_helper
FUNC_EPILOGUE

jmp     init_playerins_with_padmanip_return
init_playerins_with_padmanip_injection ENDP


EXTERN MoveMapStep_SetPlayerLockOn_FromController_offset_return: qword
EXTERN ggpoStarted_ptr: qword

PUBLIC MoveMapStep_SetPlayerLockOn_FromController_offset_injection
MoveMapStep_SetPlayerLockOn_FromController_offset_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     exit
;original code
cmp     byte ptr [rcx+1430h], 0
setz    al
mov     [rcx+1431h], al
exit:
jmp     MoveMapStep_SetPlayerLockOn_FromController_offset_return
MoveMapStep_SetPlayerLockOn_FromController_offset_injection ENDP


EXTERN followupBullet_loop_return: qword
extern followupBullet_loop_helper: proc

PUBLIC followupBullet_loop_injection
followupBullet_loop_injection PROC
floop:
MOV     RCX,qword ptr [RDI + 20h]
FUNC_PROLOGUE
mov     rcx, rdi
call    followupBullet_loop_helper
FUNC_EPILOGUE
CALL    qword ptr [Build_BulletIns_FollowupBullet]
CMP     qword ptr [RDI + 20h],0
JNZ     floop
jmp     followupBullet_loop_return
followupBullet_loop_injection ENDP



EXTERN get_item_currently_being_used_return: qword
extern get_item_currently_being_used_injection_helper: proc

PUBLIC get_item_currently_being_used_injection
get_item_currently_being_used_injection PROC
FUNC_PROLOGUE_LITE
call    get_item_currently_being_used_injection_helper
FUNC_EPILOGUE_LITE_NORAX
cmp     al, 0
je      continue
mov     rax, rdx
ret
continue:
;original code
movsxd  rax, dword ptr [rcx+1DCh]
mov     dword ptr [rdx], 0FFFFFFFFh
mov     dword ptr [rdx+4], 1
jmp     get_item_currently_being_used_return
get_item_currently_being_used_injection ENDP

_TEXT    ENDS

END
