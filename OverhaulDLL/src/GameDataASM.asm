_DATA SEGMENT

_DATA ENDS

_TEXT SEGMENT

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
