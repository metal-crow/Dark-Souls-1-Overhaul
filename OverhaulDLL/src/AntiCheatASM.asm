_DATA SEGMENT

jmp_skip_hp_set dq  14032295bh
LAB_1405088d9   dq  1405088d9h
thunk_FUN_142656a04 dq  142656a04h
LAB_140352e6f   dq  140352e6fh

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

FUNC_EPILOGUE_NORCX macro
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
    ;mov     rcx,[rsp+00000228h]
    mov     rbx,[rsp+00000220h]
    fxrstor [rsp+20h]
    mov     rsp,[rsp+00000248h]
    pop     rax
    popfq 
endm

extern dmg_guard_return: qword
extern dmg_guard_asm_check_helper: proc

PUBLIC dmg_guard_asm_check
dmg_guard_asm_check PROC
;original code
SUB     RSP,20h
TEST    byte ptr [RCX+524h],40h
MOV     RBX,RCX
jnz     label_jmp_skip_hp_set

FUNC_PROLOGUE
;target entity ptr passed implicitly as rcx
;damage amount passed implicitly as rdx
mov     r8, r15 ; r15 is entityPointer of the attacker
call    dmg_guard_asm_check_helper
mov     rcx, rax ;grab the result
FUNC_EPILOGUE_NORCX

;check the result
test    rcx, rcx
MOV     rcx, rbx ;restore rcx
jz      normal_exit
label_jmp_skip_hp_set:
jmp     qword ptr [jmp_skip_hp_set]
normal_exit:
jmp     dmg_guard_return
dmg_guard_asm_check ENDP


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

FUNC_PROLOGUE
movaps  xmmword ptr [new_player_position], xmm3 ;save the new player position to compare with
lea     rcx, [rbx+10h] ;ptr to _old_ position (b4 being changed)
call    TeleBackstabProtect_helper_check ;this will set the new PC position if needed
FUNC_EPILOGUE

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

FUNC_PROLOGUE
lea     rcx, [rsi+98h] ;ptr to loaded name string
call    NameCrash_prevent_helper
FUNC_EPILOGUE

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

FUNC_PROLOGUE
mov     rcx, rdi ;pointer to the packet data
call    ReadParseType18_packet_injection_helper
FUNC_EPILOGUE

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

FUNC_PROLOGUE
lea     rcx, [RAX+30h + 10h] ;pointer to the packet data
call    ReadParseType35_packet_injection_helper
FUNC_EPILOGUE

;original code
MOV     RDX,RBX
CMP     EAX,10h
JNZ     normal_exit
jmp     qword ptr [LAB_1405088d9]
normal_exit:
JMP     qword ptr [thunk_FUN_142656a04]
ReadParseType35_packet_injection ENDP

_TEXT    ENDS


extern ReadParseType3_packet_return: qword
extern ReadParseType3_packet_injection_helper: proc

PUBLIC ReadParseType3_packet_injection
ReadParseType3_packet_injection PROC

FUNC_PROLOGUE
lea     rcx, rax ;pointer to the target chr
call    ReadParseType3_packet_injection_helper
;preserve rax here if we want to set the position
FUNC_EPILOGUE

CMP    rax, 0
JNZ    normal_exit
jmp    qword ptr [LAB_140352e6f]
normal_exit:
;original code
MOVSD   XMM0,qword ptr [RAX + 130h]
MOV     EAX,dword ptr [RAX + 138h]
JMP     ReadParseType3_packet_return
ReadParseType35_packet_injection ENDP

_TEXT    ENDS

END
