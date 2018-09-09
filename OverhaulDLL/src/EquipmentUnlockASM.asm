_DATA SEGMENT

abort_enter dq 14062E70Fh

_DATA ENDS

_TEXT SEGMENT

extern add_check_equipment_unlock_injection_return: qword

PUBLIC main_equipment_unlock_injection
main_equipment_unlock_injection PROC

;if we're entering the weapon equip menu, check the saved check-call to see if we're allowed to
cmp     esi, 3
jg      normal_continue
test    r15b, r15b
jz      abort_weapon_enter
normal_continue:
xor     r15b, r15b ;not sure if it's safe to trash this register, but i think it should be 0, so restore it
mov     qword ptr [rsp+144], rbx
mov     qword ptr [rsp+168], rdi
jmp     add_check_equipment_unlock_injection_return
abort_weapon_enter:
xor     r15b, r15b ;not sure if it's safe to trash this register, but i think it should be 0, so restore it
jmp     qword ptr [abort_enter]

main_equipment_unlock_injection ENDP

_TEXT    ENDS

END
