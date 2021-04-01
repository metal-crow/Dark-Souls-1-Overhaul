_TEXT    SEGMENT

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

_TEXT    ENDS

END
