_DATA SEGMENT

send_PlayerGeneralInfo   dq  1403980e0h

_DATA ENDS

_TEXT    SEGMENT

EXTERN homing_spell_trigger_injection_return: qword
EXTERN world_char_base_spelldesync_asm: qword

;Triggered when a homing spell (such as homing soul mass orb) is fired off
;Forcably send a type 1 packet out at this time, so the opponent gets the current position
;This should trigger their client to also trigger the homing spell visual firing

PUBLIC homing_spell_trigger_injection
homing_spell_trigger_injection PROC

sub     rsp, 8
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

;call the network packet sending function
mov     rdx, [world_char_base_spelldesync_asm]
mov     rdx, [rdx] ;worldChrManImp
mov     rdx, [rdx+068h] ;playerIns
mov     rdx, [rdx+068h] ;playerCtrl
mov     rcx, [rdx+088h] ;Padman
mov     dword ptr [rcx + 0238h], 0 ;set ticks_remaining_before_packetsent to 0
call    qword ptr [send_PlayerGeneralInfo]

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
add     rsp, 8

;original code
movaps  xmm0, [rsp+020h]
lea     rdx, [rsp+060h]
movaps  xmm1, [rsp+030h]

jmp     homing_spell_trigger_injection_return

homing_spell_trigger_injection ENDP

_TEXT    ENDS

END
