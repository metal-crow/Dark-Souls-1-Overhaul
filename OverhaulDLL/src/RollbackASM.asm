_DATA SEGMENT

sub_1401862A0   dq  1401862A0h
nullsub_78     dq  140185470h
Build_BulletIns_FollowupBullet  dq  140fdcac0h
LAB_1409c7675   dq  1409c7675h

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

EXTERN DamageEntry_Clear_id_return: qword
extern DamageEntry_Clear_id_helper: proc

;Clear_DamageEntry, where it writes the entry's next id (the old one with the generation incremented). The helper may pick a
;newer generation; see "Entry ids across timelines" in DamageManStructFunctions.cpp
PUBLIC DamageEntry_Clear_id_injection
DamageEntry_Clear_id_injection PROC
;original code
mov     dword ptr [rbx+40h], 0FFFFFFFFh
movzx   ecx, ax
or      edx, ecx

FUNC_PROLOGUE
mov     rcx, rbx
call    DamageEntry_Clear_id_helper
mov     [rsp+00000230h], rax ;use FUNC_EPILOGUE to put this result in RDX
FUNC_EPILOGUE

;original code
lea     rcx, [rsp+20h]
jmp     DamageEntry_Clear_id_return
DamageEntry_Clear_id_injection ENDP

;WorldFreeze.h: while a rollback session runs, the object steps get a frame time of 0, so no object animates, moves or counts down.
;Each call site is movaps xmm1, <frame time>; mov rcx, [manager]; call <step> (15 bytes)
EXTERN rollback_world_frozen: byte

EXTERN WorldObjMan_step_live_return: qword
PUBLIC WorldObjMan_step_live_injection
WorldObjMan_step_live_injection PROC
;original code: movaps xmm1, xmm7 (the frame time); mov rcx, [manager]; call step
movaps  xmm1, xmm7
cmp     byte ptr [rollback_world_frozen], 0
je      @F
xorps   xmm1, xmm1
@@:
mov     rax, 141c75dd8h
mov     rcx, qword ptr [rax]
mov     rax, 14030c9b0h
call    rax
jmp     WorldObjMan_step_live_return
WorldObjMan_step_live_injection ENDP

EXTERN WorldObjMan_step_objs_return: qword
PUBLIC WorldObjMan_step_objs_injection
WorldObjMan_step_objs_injection PROC
;original code: movaps xmm1, xmm6 (the frame time); mov rcx, [manager]; call step
movaps  xmm1, xmm6
cmp     byte ptr [rollback_world_frozen], 0
je      @F
xorps   xmm1, xmm1
@@:
mov     rax, 141c75dd8h
mov     rcx, qword ptr [rax]
mov     rax, 14030b6c0h
call    rax
jmp     WorldObjMan_step_objs_return
WorldObjMan_step_objs_injection ENDP

EXTERN WorldObjMan_step_objs_post_return: qword
PUBLIC WorldObjMan_step_objs_post_injection
WorldObjMan_step_objs_post_injection PROC
;original code: movaps xmm1, xmm6 (the frame time); mov rcx, [manager]; call step
movaps  xmm1, xmm6
cmp     byte ptr [rollback_world_frozen], 0
je      @F
xorps   xmm1, xmm1
@@:
mov     rax, 141c75dd8h
mov     rcx, qword ptr [rax]
mov     rax, 14030b820h
call    rax
jmp     WorldObjMan_step_objs_post_return
WorldObjMan_step_objs_post_injection ENDP

EXTERN WorldObjActMan_step_return: qword
PUBLIC WorldObjActMan_step_injection
WorldObjActMan_step_injection PROC
;original code: movaps xmm1, xmm6 (the frame time); mov rcx, [manager]; call step
movaps  xmm1, xmm6
cmp     byte ptr [rollback_world_frozen], 0
je      @F
xorps   xmm1, xmm1
@@:
mov     rax, 141c75e40h
mov     rcx, qword ptr [rax]
mov     rax, 140316510h
call    rax
jmp     WorldObjActMan_step_return
WorldObjActMan_step_injection ENDP

EXTERN DamageMan_EntryCount_return: qword
extern rollback_damage_attacker_allowed: proc

;FUN_1403ca770(DamageMan*, params, attacker), which says how many damage entries to create. While the world is frozen an attacker
;that is not a session player gets 0, the game's own "no entry" path (its caller then returns id -1)
PUBLIC DamageMan_EntryCount_injection
DamageMan_EntryCount_injection PROC
cmp     byte ptr [rollback_world_frozen], 0
je      entrycount_original
push    rcx
push    rdx
push    r8
push    r9
sub     rsp, 28h
mov     rcx, r8
call    rollback_damage_attacker_allowed
add     rsp, 28h
pop     r9
pop     r8
pop     rdx
pop     rcx
test    al, al
jnz     entrycount_original
xor     eax, eax
ret
entrycount_original:
;original code
mov     qword ptr [rsp+8], rbx
push    rdi
sub     rsp, 30h
mov     rbx, r8
mov     rdi, rdx
jmp     DamageMan_EntryCount_return
DamageMan_EntryCount_injection ENDP

EXTERN DamageEntry_HitChr_return: qword
extern rollback_damage_defender_allowed: proc

;FUN_1403c7520(DamageEntry*, defender ChrIns*, ...): a damage entry hitting a character. While the world is frozen a defender that
;is not a session player is not hit at all (the frozen characters keep their hit records, which rollback does not restore)
PUBLIC DamageEntry_HitChr_injection
DamageEntry_HitChr_injection PROC
;original code
test    rdx, rdx
jz      hitchr_skip
cmp     byte ptr [rollback_world_frozen], 0
je      hitchr_original
push    rcx
push    rdx
push    r8
push    r9
sub     rsp, 28h
call    rollback_damage_defender_allowed
add     rsp, 28h
pop     r9
pop     r8
pop     rdx
pop     rcx
test    al, al
jz      hitchr_skip
hitchr_original:
;original code
mov     r11, rsp
push    rbp
push    rbx
jmp     DamageEntry_HitChr_return
hitchr_skip:
ret
DamageEntry_HitChr_injection ENDP



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



EXTERN simpleshapephantom_collisionDetails_iterate_return: qword

PUBLIC simpleshapephantom_collisionDetails_iterate_injection
simpleshapephantom_collisionDetails_iterate_injection PROC
MOV        RDX,qword ptr [RAX + RBX*8]
CMP        RDX, 0
JNE        nonnull
JMP        qword ptr [LAB_1409c7675]
nonnull:
MOV        RAX,qword ptr [R15 + 20h]
MOVZX      ECX,byte ptr [RAX + 10h]
MOV        RAX,qword ptr [RDX]
jmp        simpleshapephantom_collisionDetails_iterate_return
simpleshapephantom_collisionDetails_iterate_injection ENDP


EXTERN Destruct_SFXEntry_return: qword
extern OnSfxEntryDestruct: proc

PUBLIC Destruct_SFXEntry_injection
Destruct_SFXEntry_injection PROC
FUNC_PROLOGUE_LITE
; RCX = SFXEntry* (first param of Destruct_SFXEntry)
call    OnSfxEntryDestruct
FUNC_EPILOGUE_LITE_NORAX
; AL = true if we should skip destruction
test    al, al
jnz     skip_destruct
; Replay displaced instructions and continue into Destruct_SFXEntry body
push    rdi
sub     rsp, 30h
mov     qword ptr [rsp+20h], -2
jmp     Destruct_SFXEntry_return
skip_destruct:
; Destruct_SFXEntry returns param_1 in RAX
mov     rax, rcx
ret
Destruct_SFXEntry_injection ENDP


EXTERN Destruct_FxBehaviorNode_return: qword
extern OnFxBehaviorNodeDealloc: proc

PUBLIC Destruct_FxBehaviorNode_injection
Destruct_FxBehaviorNode_injection PROC
FUNC_PROLOGUE_LITE
; RCX = Unused. Easier to not bother reordering
; RDX = FxBehaviorNode* (second param of Destruct_SFXEntry)
call    OnFxBehaviorNodeDealloc
FUNC_EPILOGUE_LITE_NORAX
; AL = true if we should skip destruction
test    al, al
jnz     skip_destruct
MOV     qword ptr [RSP + 8], RBX
MOV     qword ptr [RSP + 10h], RSI
PUSH    RDI
SUB     RSP,20h
jmp     Destruct_FxBehaviorNode_return
skip_destruct:
ret
Destruct_FxBehaviorNode_injection ENDP


EXTERN ChrCam_ApplyToPadManipulator_return: qword
EXTERN rollback_live_camera: xmmword

;Apply_ChrCam_To_PlayerInsPadManipulator's store of the camera angles into the viewing player's ChrManipulator (+0x50).
;While a rollback session runs the PadManipulator's camera angles are input, set from each player's RollbackInput, so the
;live camera goes to rollback_live_camera instead (PackRollbackInput reads it), and live and re-simulated frames match.
PUBLIC ChrCam_ApplyToPadManipulator_injection
ChrCam_ApplyToPadManipulator_injection PROC
;original code
mov     rbx, qword ptr [rsp+40h]
mov     rcx, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rcx], 0
jne     to_side_buffer
;original code
movaps  xmmword ptr [rax+50h], xmm6
jmp     chrcam_done
to_side_buffer:
movups  xmmword ptr [rollback_live_camera], xmm6
chrcam_done:
;original code
movaps  xmm6, xmmword ptr [rsp+20h]
jmp     ChrCam_ApplyToPadManipulator_return
ChrCam_ApplyToPadManipulator_injection ENDP


EXTERN ResonanceMagic_Arm_return: qword

;FUN_14080cbb0: arms the local player's ChrResonanceMagicSlot every frame. Armed, the slot rolls a resonance level with the
;game's global RNG every 3 s, applies SpEffect 40-44 to its own character only and sends a type 34 packet: local, random
;and network-timed, so it can never be the same on two machines. Not armed while a rollback session runs.
PUBLIC ResonanceMagic_Arm_injection
ResonanceMagic_Arm_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     resonance_disabled
;original code
push    rdi
sub     rsp, 30h
mov     qword ptr [rsp+20h], -2
jmp     ResonanceMagic_Arm_return
resonance_disabled:
ret
ResonanceMagic_Arm_injection ENDP


EXTERN Step_Chr_canonical_order_return: qword
extern Step_Chr_canonical_order_helper: proc

;Step_Chr_sub2, after it has linked this frame's always-stepped characters (R12, {ChrIns*, frame delta, next}) and before
;the loops that step them. The helper puts the session players into the same order on every machine.
PUBLIC Step_Chr_canonical_order_injection
Step_Chr_canonical_order_injection PROC
FUNC_PROLOGUE
mov     rcx, r12
call    Step_Chr_canonical_order_helper
FUNC_EPILOGUE

;original code
mov     word ptr [r14+1F2Dh], 0
mov     rax, 140186340h
call    rax
jmp     Step_Chr_canonical_order_return
Step_Chr_canonical_order_injection ENDP


;---- Inventory changes the session's input cannot carry -------------------------------------------------------------------
;The input only says which items a character has equipped and is using; every other change to an inventory has to happen on
;every machine on the same frames. Picking up, being awarded, dropping or boxing an item happens on one machine only (the
;menus and the world are not simulated on the others), and the inventory is rolled back while the world event that caused
;it is not, so while a session runs these are refused at their source, before anything is consumed.

extern rollback_item_change_blocked: proc

;rollback_item_change_blocked(which), with the stack aligned for the call. Clobbers the volatile registers.
ITEM_CHANGE_BLOCKED macro which
    push    rbp
    mov     rbp, rsp
    and     rsp, -10h
    sub     rsp, 20h
    mov     ecx, which
    call    rollback_item_change_blocked
    mov     rsp, rbp
    pop     rbp
endm

EXTERN MapItem_Pickup_return: qword

;FUN_1403fb144's lookup of the map item the player picks up: call FUN_1403f8600 / mov rdx,rax / test rax,rax / jz (to the
;epilogue). During a session the lookup finds nothing, so neither the local pickup (remove from the map, award) nor the
;request to the session host happens, and the item stays where it is.
PUBLIC MapItem_Pickup_injection
MapItem_Pickup_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     mapitem_pickup_blocked
;original code
mov     rax, 1403f8600h
call    rax
mov     rdx, rax
test    rax, rax
jz      mapitem_pickup_none
jmp     MapItem_Pickup_return
mapitem_pickup_blocked:
ITEM_CHANGE_BLOCKED 0
mapitem_pickup_none:
mov     rax, 1403fb1f3h
jmp     rax
MapItem_Pickup_injection ENDP

EXTERN AwardItemLot_return: qword

;FUN_1403fc940, the event scripts' AwardItemLot: awards the lot to the local player, then sets the lot's event flag
PUBLIC AwardItemLot_injection
AwardItemLot_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     awarditemlot_blocked
;original code
mov     rax, rsp
push    rbp
push    rsi
push    rdi
push    r12
push    r13
push    r14
push    r15
jmp     AwardItemLot_return
awarditemlot_blocked:
ITEM_CHANGE_BLOCKED 1
ret
AwardItemLot_injection ENDP

EXTERN LuaAddInventoryItem_return: qword

;FUN_1404d5a00, behind the event scripts' AddInventoryItem: gives an item to the local player
PUBLIC LuaAddInventoryItem_injection
LuaAddInventoryItem_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     luaadditem_blocked
;original code
mov     rax, rsp
push    rbp
push    rsi
push    rdi
push    r12
push    r13
push    r14
push    r15
jmp     LuaAddInventoryItem_return
luaadditem_blocked:
ITEM_CHANGE_BLOCKED 2
ret
LuaAddInventoryItem_injection ENDP

EXTERN ItemDrop_return: qword

;FUN_1406ae920(FrpgMenuDlgInventory*, inventory index): the inventory menu's drop. Takes the item out of the local player's
;inventory and puts it on the ground as a map item
PUBLIC ItemDrop_injection
ItemDrop_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     itemdrop_blocked
;original code
mov     qword ptr [rsp+20h], rbx
push    rbp
push    rsi
push    rdi
push    r12
push    r13
push    r14
jmp     ItemDrop_return
itemdrop_blocked:
ITEM_CHANGE_BLOCKED 3
ret
ItemDrop_injection ENDP

EXTERN BottomlessBoxDeposit_return: qword

;FUN_1406e0270(FrpgMenuDlgInventory*, bool): puts an inventory item in the Bottomless Box
PUBLIC BottomlessBoxDeposit_injection
BottomlessBoxDeposit_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     boxdeposit_blocked
;original code
mov     byte ptr [rsp+10h], dl
push    rbp
push    rbx
push    rsi
push    rdi
push    r13
push    r14
push    r15
jmp     BottomlessBoxDeposit_return
boxdeposit_blocked:
ITEM_CHANGE_BLOCKED 4
ret
BottomlessBoxDeposit_injection ENDP

EXTERN BottomlessBoxWithdraw_return: qword

;FUN_140768220(RepositoryData*, EquipGameData*, index, quantity): takes an item out of the Bottomless Box. Returns whether it did
PUBLIC BottomlessBoxWithdraw_injection
BottomlessBoxWithdraw_injection PROC
mov     rax, qword ptr [ggpoStarted_ptr]
cmp     byte ptr [rax], 0
jne     boxwithdraw_blocked
;original code
mov     r11, rsp
push    rbx
push    rbp
push    rsi
push    r13
sub     rsp, 48h
movsxd  rbx, r8d
jmp     BottomlessBoxWithdraw_return
boxwithdraw_blocked:
ITEM_CHANGE_BLOCKED 5
xor     eax, eax
ret
BottomlessBoxWithdraw_injection ENDP

_TEXT    ENDS

END
