_DATA SEGMENT

FUN_140322a98   dq  140322a98h
jmp_skip_hp_set dq  14032295bh
LAB_1405088d9   dq  1405088d9h
thunk_FUN_142656a04 dq  142656a04h

_DATA ENDS

_TEXT    SEGMENT

extern npc_guard_check_exit: qword
extern npc_guard_asm_check_helper: proc

PUBLIC npc_guard_asm_check
npc_guard_asm_check PROC
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

mov     rcx, r15 ; r15 is entityPointer of the attacker
mov     rdx, rbx ; rbx is entityPointer of target; check if they are a non-hostile NPC
mov     r8d, edi ; current damage amount
call    npc_guard_asm_check_helper
mov     edi, eax ;use the return value as the new damage amount

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
MOV     EDX,dword ptr [RBX + 3e8h]
MOV     RCX,RBX
ADD     EDX,EDI
jmp     qword ptr [FUN_140322a98]
npc_guard_asm_check ENDP


extern boss_guard_return: qword
extern boss_guard_asm_check_helper: proc

PUBLIC boss_guard_asm_check
boss_guard_asm_check PROC
;original code
SUB     RSP,20h
TEST    byte ptr [RCX+524h],40h
MOV     RBX,rcx
jnz     label_jmp_skip_hp_set

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
push    rax
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

;boss entity ptr passed implicitly as rcx
call    boss_guard_asm_check_helper
mov     rcx, rax ;grab the result

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rax
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

;check the result
test    rcx, rcx
MOV     rcx, rbx ;restore rcx
jz      normal_exit
label_jmp_skip_hp_set:
jmp     qword ptr [jmp_skip_hp_set]
normal_exit:
jmp     boss_guard_return
boss_guard_asm_check ENDP


extern TeleBackstabProtect_setPosition_return: qword
extern TeleBackstabProtect_helper_check: proc
extern new_player_position: qword

extern saved_anim_id: dword
extern TeleBackstabProtect_store_AnimationId_return: qword

PUBLIC TeleBackstabProtect_setPosition_check
TeleBackstabProtect_setPosition_check PROC

;original code
movss   xmm3, dword ptr [rsp+20h]
unpcklps xmm3, xmm0
unpcklps xmm2, xmm1
unpcklps xmm3, xmm2

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

movaps  xmmword ptr [new_player_position], xmm3 ;save the new player position to compare with
lea     rcx, [rbx+10h] ;ptr to _old_ position (b4 being changed)
call    TeleBackstabProtect_helper_check ;this will set the new PC position if needed

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
jmp     TeleBackstabProtect_setPosition_return

TeleBackstabProtect_setPosition_check ENDP


;injected into the function that sets the player's animation ID to a backstab
PUBLIC TeleBackstabProtect_store_AnimationId
TeleBackstabProtect_store_AnimationId PROC

;original code
mov     rax, [rsi+10h]
test    rax, rax
jz      dont_set
mov     edi, [rax+28h] ;sets the animation id to backstab
mov     saved_anim_id, edi ;save animation id
dont_set:
mov     eax, edi
jmp     TeleBackstabProtect_store_AnimationId_return

TeleBackstabProtect_store_AnimationId ENDP


extern NameCrash_prevent_return: qword
extern NameCrash_prevent_helper: proc

PUBLIC NameCrash_prevent
NameCrash_prevent PROC
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

lea     rcx, [rsi+98h] ;ptr to loaded name string
call    NameCrash_prevent_helper

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
mov     rbx, [rsp+30h]
mov     rsi, [rsp+38h]
add     rsp, 20h
jmp     NameCrash_prevent_return
NameCrash_prevent ENDP


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

mov     rcx, rdi ;pointer to the packet data
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
CMP     EAX, 80h
PUSH    RBP
MOV     RBP, 142fdd7e6h
jmp     ReadParseType18_packet_return
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

lea     rcx, [RSP + 30h] ;pointer to the packet data
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
MOV     RDX,RBX
CMP     EAX,10h
JNZ     normal_exit
jmp     qword ptr [LAB_1405088d9]
normal_exit:
JMP     qword ptr [thunk_FUN_142656a04]
ReadParseType35_packet_injection ENDP

_TEXT    ENDS

END
