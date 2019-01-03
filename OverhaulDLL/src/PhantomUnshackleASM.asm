_TEXT    SEGMENT

EXTERN mp_zone_changing_injection_return: qword
EXTERN vanilla_mp_zone: dword
EXTERN world_char_base_asm: qword

; Assembly code below is injected into the function that changes the the current mp zone
; This edit prevents the mp zone from being changed to whatever it was before
; This allows the thread to have full control of the mp zone
; Also saves the intended mp zone

PUBLIC mp_zone_changing_injection
mp_zone_changing_injection PROC

push    rcx
mov     rcx, [world_char_base_asm]
mov     rcx, [rcx]
mov     rcx, [rcx + 68h] ;get address of struct containing mp id for PC
cmp     rcx, rbx ;check if the ptr being written to is for player
jne     exit
mov     vanilla_mp_zone, eax
exit:
pop     rcx

;original code
mov     rcx, rbx
mov     eax, [rdi+0Ch]
mov     [rbx+358h], eax

jmp     mp_zone_changing_injection_return

mp_zone_changing_injection ENDP

_TEXT    ENDS

END
