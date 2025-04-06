_DATA SEGMENT

sub_140426ba0   dq  140426ba0h
FUN_140504620   dq  140504620h
LAB_1405045b5   dq  1405045b5h

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
mov     ecx, dword ptr [RAX+20h + 10h] ;get the bullet target entity id
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
FUNC_PROLOGUE
call    type1_p2pPacket_parse_injection_helper_function
FUNC_EPILOGUE
jmp     qword ptr [LAB_1405045b5]

;original code
normal_exit:
LEA     RDX,[RSP + 30h]
MOV     RCX,RBX
CALL    qword ptr [FUN_140504620]
TEST    AL,AL
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
lea     rdx, dword ptr [RAX+20h + 10h]; bulletParamEntry.target_id
call    homing_spell_checkIfTriggered_injection_helper_function
FUNC_EPILOGUE

jmp     homing_spell_checkIfTriggered_injection_return
homing_spell_checkIfTriggered_injection ENDP


_TEXT    ENDS

END
