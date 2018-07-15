_DATA SEGMENT

_DATA ENDS

_TEXT SEGMENT

extern npc_guard_WorldChrBase: qword
extern npc_guard_check_exit: qword

PUBLIC npc_guard_asm_check
npc_guard_asm_check PROC

; r15 is entityPointer of the attacker
; If local player is attacking, do not protect NPCs
push rax
mov  rax, QWORD PTR [npc_guard_WorldChrBase]
mov  rax, [rax]
mov  rax, [rax + 68h] ; rax is now entityPointer to local player
cmp  rax, r15 ; If entityPointer for local player == attacker
pop  rax
je bypass_check

; rbx is entityPointer of target; check if they are a non-hostile NPC
push rax
mov  rax, [rbx + 578h] ;Event Entity ID for the target entity

;Check for event ID of a protected NPC
cmp rax, 1798h ; Griggs of Vinheim (Undead Burg)
je abort_damage
cmp rax, 179Ah ; Griggs of Vinheim (Firelink Shrine)
je abort_damage
cmp rax, 184Ch ; Blacksmith Rickert
je abort_damage
cmp rax, 178Eh ; Big Hat Logan
je abort_damage
cmp rax, 178Fh ; Big Hat Logan (The Duke's Archives)
je abort_damage
cmp rax, 17A2h ; Dusk of Oolacile
je abort_damage
cmp rax, 1A2Ch ; Dusk of Oolacile (Past forest)
je abort_damage
cmp rax, 1824h ; Ingward (New Londo)
je abort_damage
cmp rax, 1825h ; Ingward (Firelink Shrine)
je abort_damage
cmp rax, 17F2h ; Laurentius (Depths)
je abort_damage
cmp rax, 17F3h ; Laurentius (Firelink Shrine)
je abort_damage
cmp rax, 181Ah ; Quelana of Izalith
je abort_damage
cmp rax, 1810h ; Eingyi
je abort_damage
cmp rax, 17C0h ; Petrus of Thorolund
je abort_damage
cmp rax, 17C1h ; Petrus of Thorolund
je abort_damage
cmp rax, 17B6h ; Rhea of Thorolund
je abort_damage
cmp rax, 187Eh ; Crestfallen Warrior
je abort_damage
cmp rax, 43238h ; Anastacia
je abort_damage
cmp rax, 82208h ; Kingseeker Frampt
je abort_damage
cmp rax, 17CAh ; Vince of Thorolund
je abort_damage
cmp rax, 17D4h ; Nico of Thorolund
je abort_damage
cmp rax, 1770h ; Solaire of Astora
je abort_damage
cmp rax, 1771h ; Solaire of Astora (Shrine in Undead Burg?)
je abort_damage
cmp rax, 1772h ; Solaire of Astora (Anor Londo)
je abort_damage
cmp rax, 1773h ; Solaire of Astora (Demon Ruins/Lost Izalith)
je abort_damage
cmp rax, 1774h ; Solaire of Astora (Demon Ruins/Lost Izalith; heliozoan?)
je abort_damage
cmp rax, 1775h ; Solaire of Astora
je abort_damage
cmp rax, 1776h ; Solaire of Astora
je abort_damage
cmp rax, 1777h ; Solaire of Astora
je abort_damage
cmp rax, 1778h ; Solaire of Astora
je abort_damage
cmp rax, 1856h ; Undead Male Merchant
je abort_damage
cmp rax, 1857h ; Undead Male Merchant
je abort_damage
cmp rax, 1858h ; Undead Male Merchant
je abort_damage
cmp rax, 3D478h ; Undead Merchant
je abort_damage
cmp rax, 3D479h ; Undead Merchant
je abort_damage
cmp rax, 40740h ; Andre of Astora
je abort_damage
cmp rax, 189Ch ; Knight Lautrec of Carim
je abort_damage
cmp rax, 189Dh ; Knight Lautrec of Carim
je abort_damage
cmp rax, 18E2h ; Oswald of Carim
je abort_damage
cmp rax, 82E24h ; Alvina (No textures)
je abort_damage
cmp rax, 82E88h ; Alvina (No textures gray)
je abort_damage
cmp rax, 18A6h ; Shiva of the East
je abort_damage
cmp rax, 1874h ; Domhnall of Zena (Depths)
je abort_damage
cmp rax, 1875h ; Domhnall of Zena (Firelink Shrine)
je abort_damage
cmp rax, 825F0h ; The Fair Lady
je abort_damage
cmp rax, 1888h ; Siegmeyer of Catarina (Sen's Fortress)
je abort_damage
cmp rax, 1889h ; Siegmeyer of Catarina (Anor Londo)
je abort_damage
cmp rax, 188Ah ; Siegmeyer of Catarina (Firelink Shrine)
je abort_damage
cmp rax, 188Bh ; Siegmeyer of Catarina (Blighttown)
je abort_damage
cmp rax, 188Ch ; Siegmeyer of Catarina (Lost Izalith)
je abort_damage
cmp rax, 1892h ; Sieglinde of Catarina (The Duke's Archives)
je abort_damage
cmp rax, 1893h ; Sieglinde of Catarina (The GreHollow)
je abort_damage
cmp rax, 186Ah ; Crestfallen Merchant
je abort_damage
cmp rax, 82209h ; Kaathe
je abort_damage
cmp rax, 177Ah ; Lady of the Darkling
je abort_damage
cmp rax, 81A38h ; Gwynevere
je abort_damage
cmp rax, 45D31h ; Giant Blacksmith
je abort_damage
cmp rax, 474A0h ; Vamos
je abort_damage
cmp rax, 474A2h ; Vamos
je abort_damage
cmp rax, 65130h ; Elizabeth
je abort_damage
cmp rax, 65130h ; Elizabeth (No textures)
je abort_damage
cmp rax, 3F3B8h ; Marvellous Chester
je abort_damage
cmp rax, 1A54h ; Lord's Blade Ciaran
je abort_damage
cmp rax, 1A55h ; Lord's Blade Ciaran
je abort_damage
cmp rax, 1A56h ; Lord's Blade Ciaran
je abort_damage
cmp rax, 1A57h ; Lord's Blade Ciaran
je abort_damage
cmp rax, 1A58h ; Lord's Blade Ciaran
je abort_damage
cmp rax, 1A59h ; Lord's Blade Ciaran
je abort_damage
cmp rax, 64578h ; Hawkeye Gough
je abort_damage
cmp rax, 6476Ch ; Hawkeye Gough (No textures)
je abort_damage

pop rax
jmp bypass_check

abort_damage:
pop rax
; Set damage to 0
mov edi, 0

bypass_check:
jmp     npc_guard_check_exit

npc_guard_asm_check ENDP

_TEXT    ENDS

END
