_DATA SEGMENT

sub_140BCEDB0   dq  140BCEDB0h
sub_140BA8400   dq  140BA8400h

_DATA ENDS

_TEXT SEGMENT

extern game_write_playerdata_to_flatbuffer_injection_helper: PROC
extern game_write_playerdata_to_flatbuffer_injection_return: qword

PUBLIC game_write_playerdata_to_flatbuffer_injection
game_write_playerdata_to_flatbuffer_injection PROC

;save temp registers
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
;push    r9
push    r10
push    r11
sub     rsp, 20h

mov     rdx, r9 ;array length
mov     rcx, r8 ;array start
call    game_write_playerdata_to_flatbuffer_injection_helper
mov     r9, rax ;save over the length

add     rsp, 20h
pop     r11
pop     r10
;pop     r9
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

;original code
lea     rdx, [rsp+24h]
mov     rcx, rbx
call    qword ptr [sub_140BCEDB0]

jmp     game_write_playerdata_to_flatbuffer_injection_return

game_write_playerdata_to_flatbuffer_injection ENDP


extern PlayerStat_ClearCount_injection_return: qword
PUBLIC PlayerStat_ClearCount_injection
PlayerStat_ClearCount_injection PROC
;set the clear count to 1
;so players can have all NG+ stuff without being detected as banned
mov     eax, 1
;original code
mov     r8d, eax
mov     edx, 1
mov     rcx, rdi
jmp     PlayerStat_ClearCount_injection_return
PlayerStat_ClearCount_injection ENDP

extern PlayerStat_New_Name_5_injection_return: qword
PUBLIC PlayerStat_New_Name_5_injection
PlayerStat_New_Name_5_injection PROC
;unknown. 0 seems to be normal
mov     r8d, 0
;original code
lea     edx, [r12+3]
mov     rcx, rdi
jmp     PlayerStat_New_Name_5_injection_return
PlayerStat_New_Name_5_injection ENDP

extern PlayerStat_New_Name_7_injection_return: qword
PUBLIC PlayerStat_New_Name_7_injection
PlayerStat_New_Name_7_injection PROC
;unknown. 0xFDE80 seems to be normal
mov     eax, FDE80h
;original code
mov     r8d, eax
lea     edx, [r12+5]
mov     rcx, rdi
jmp     PlayerStat_New_Name_7_injection_return
PlayerStat_New_Name_7_injection ENDP

extern PlayerStat_SoulCount_injection_return: qword
PUBLIC PlayerStat_SoulCount_injection
PlayerStat_SoulCount_injection PROC
;a healthy 2 billion (enough to level up to max)
;this fits into an int32_t, just to be safe
mov     r8d, 2000000000
;original code
lea     edx, [r12+7]
mov     rcx, rdi
jmp     PlayerStat_SoulCount_injection_return
PlayerStat_SoulCount_injection ENDP

extern PlayerStat_SoulMemory_injection_return: qword
PUBLIC PlayerStat_SoulMemory_injection
PlayerStat_SoulMemory_injection PROC
;keep it the same value as current soul count
mov     r8d, 2000000000
;original code
lea     edx, [r12+8]
mov     rcx, rdi
jmp     PlayerStat_SoulMemory_injection_return
PlayerStat_SoulMemory_injection ENDP

extern PlayerStat_Archetype_injection_return: qword
PUBLIC PlayerStat_Archetype_injection
PlayerStat_Archetype_injection PROC
;hardcode as pyrocmancer (since they can start at sl1)
;this is ok because we're just going to hardcode all the other stat levels as well
mov     r8d, 7
;original code
lea     edx, [r12+9]
mov     rcx, rdi
jmp     PlayerStat_Archetype_injection_return
PlayerStat_Archetype_injection ENDP

extern PlayerStat_HP_injection_return: qword
PUBLIC PlayerStat_HP_injection
PlayerStat_HP_injection PROC
;default pyromancer hp
mov     r8d, 573
;original code
lea     edx, [r12+0Ah]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_HP_injection_return
PlayerStat_HP_injection ENDP

extern PlayerStat_MaxHp_injection_return: qword
PUBLIC PlayerStat_MaxHp_injection
PlayerStat_MaxHp_injection PROC
;default pyromancer hp
mov     r8d, 573
;original code
lea     edx, [r12+0Bh]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_MaxHp_injection_return
PlayerStat_MaxHp_injection ENDP

extern PlayerStat_BaseMaxHp_1_injection_return: qword
PUBLIC PlayerStat_BaseMaxHp_1_injection
PlayerStat_BaseMaxHp_1_injection PROC
;default pyromancer hp
mov     r8d, 573
;original code
lea     edx, [r12+0Ch]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_BaseMaxHp_1_injection_return
PlayerStat_BaseMaxHp_1_injection ENDP

extern PlayerStat_Mp_injection_return: qword
PUBLIC PlayerStat_Mp_injection
PlayerStat_Mp_injection PROC
;default pyromancer mp
mov     r8d, 81
;original code
lea     edx, [r12+0Dh]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_Mp_injection_return
PlayerStat_Mp_injection ENDP

extern PlayerStat_MaxMp_injection_return: qword
PUBLIC PlayerStat_MaxMp_injection
PlayerStat_MaxMp_injection PROC
;default pyromancer mp
mov     r8d, 81
;original code
lea     edx, [r12+0Eh]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_MaxMp_injection_return
PlayerStat_MaxMp_injection ENDP

extern PlayerStat_BaseMaxHp_2_injection_return: qword
PUBLIC PlayerStat_BaseMaxHp_2_injection
PlayerStat_BaseMaxHp_2_injection PROC
;default pyromancer hp
mov     r8d, 573
;original code
lea     edx, [r12+0Fh]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_BaseMaxHp_2_injection_return
PlayerStat_BaseMaxHp_2_injection ENDP

extern PlayerStat_Vitality_injection_return: qword
PUBLIC PlayerStat_Vitality_injection
PlayerStat_Vitality_injection PROC
;default pyromancer vit
mov     r8d, 10
;original code
lea     edx, [r12+10h]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_Vitality_injection_return
PlayerStat_Vitality_injection ENDP

extern PlayerStat_Attunement_injection_return: qword
PUBLIC PlayerStat_Attunement_injection
PlayerStat_Attunement_injection PROC
;default pyromancer att
mov     r8d, 12
;original code
lea     edx, [r12+11h]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_Attunement_injection_return
PlayerStat_Attunement_injection ENDP

extern PlayerStat_Endurance_injection_return: qword
PUBLIC PlayerStat_Endurance_injection
PlayerStat_Endurance_injection PROC
;default pyromancer end
mov     r8d, 11
;original code
lea     edx, [r12+12h]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_Endurance_injection_return
PlayerStat_Endurance_injection ENDP

extern PlayerStat_Strength_injection_return: qword
PUBLIC PlayerStat_Strength_injection
PlayerStat_Strength_injection PROC
;default pyromancer str
mov     r8d, 12
;original code
lea     edx, [r12+13h]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_Strength_injection_return
PlayerStat_Strength_injection ENDP

extern PlayerStat_Dexterity_injection_return: qword
PUBLIC PlayerStat_Dexterity_injection
PlayerStat_Dexterity_injection PROC
;default pyromancer dex
mov     r8d, 9
;original code
lea     edx, [r12+14h]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_Dexterity_injection_return
PlayerStat_Dexterity_injection ENDP

extern PlayerStat_Resistance_injection_return: qword
PUBLIC PlayerStat_Resistance_injection
PlayerStat_Resistance_injection PROC
;default pyromancer res
mov     r8d, 12
;original code
lea     edx, [r12+15h]
mov     rcx, rdi
call    qword ptr [sub_140BA8400]
jmp     PlayerStat_Resistance_injection_return
PlayerStat_Resistance_injection ENDP

_TEXT    ENDS

END
