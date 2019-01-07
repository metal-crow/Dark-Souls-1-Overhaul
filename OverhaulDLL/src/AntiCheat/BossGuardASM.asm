_DATA   SEGMENT

jmp_skip_hp_set dq 14032070Bh

_DATA   ENDS

_TEXT    SEGMENT

EXTERN boss_guard_return: qword

PUBLIC boss_guard_asm_check
boss_guard_asm_check PROC

;original code
sub     rsp, 20h
mov     rbx, rcx

cmp     qword ptr [rcx + 2B0h], 1600800 ;Check the Entity Id. 4 kings don't use EnableLogic
je      dont_check
push    rax
mov     rax, qword ptr [rcx + 68h]
mov     rax, qword ptr [rax + 100h] ;Load flag info for entity
test    rax, 1 ; Bit 00000001 is EnableLogic for boss
pop     rax
jz      abort_damage ; If result EnableLogic & 1 is 0, abort
dont_check:
;original code
test    byte ptr [rcx+524h], 40h

jmp     boss_guard_return
abort_damage:
jmp     qword ptr [jmp_skip_hp_set]

boss_guard_asm_check ENDP

_TEXT    ENDS

END
