_DATA SEGMENT

animation_mediator_call1    dq  14037C6F0h

_DATA ENDS

_TEXT SEGMENT

extern player_animation_mediator_cptr: qword
extern player_animation_mediator_loading_injection_return: qword

PUBLIC player_animation_mediator_loading_injection
player_animation_mediator_loading_injection PROC

;save animation mediator
push    rbx
mov     rbx, qword ptr [player_animation_mediator_cptr]
mov     qword ptr [rbx], rax
pop     rbx

;original code
mov     [rdi+20h], rax
mov     rcx, rdi
call    qword ptr[animation_mediator_call1]
mov     rax, [rdi]

jmp     player_animation_mediator_loading_injection_return

player_animation_mediator_loading_injection ENDP


extern calculate_attack_damage_injection_return: qword
extern last_attack_weaponid: dword

PUBLIC calculate_attack_damage_injection
calculate_attack_damage_injection PROC

;save the weapon id from the struct
mov         eax, dword ptr [RSI + 048h] ;weapon id
mov         last_attack_weaponid, eax

;original code
CVTTSS2SI   EAX,XMM6
mov         dword ptr [RSI + 17Ch], EAX
cvttss2si   eax, dword ptr [rbp-68h]

jmp     calculate_attack_damage_injection_return

calculate_attack_damage_injection ENDP

_TEXT    ENDS

END
