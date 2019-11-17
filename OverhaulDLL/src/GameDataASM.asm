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

_TEXT    ENDS

END
