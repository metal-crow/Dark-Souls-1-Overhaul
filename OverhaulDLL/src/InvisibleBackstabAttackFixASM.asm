_DATA SEGMENT

sendType16NetMessage    dq  1405056A0h
WorldChrManImp          dq  141d151b0h
send_PlayerGeneralInfo  dq  1403980e0h
set_ezStateActiveState  dq  1405032D0h

_DATA ENDS

_TEXT SEGMENT

extern send_throw_packet_injection_return: qword
extern force_sending_ezstate_packet_flag: qword

PUBLIC send_throw_packet_injection
send_throw_packet_injection PROC
;original code
mov     rcx, [r14+88h]
mov     rdx, 141AC8218h
call    qword ptr [sendType16NetMessage]


push    rcx
push    rdx
;get PlayerCtrl
mov     rdx, qword ptr [WorldChrManImp]
mov     rdx, qword ptr [rdx]
mov     rdx, qword ptr [rdx+68h]
mov     rdx, qword ptr [rdx+68h]
;get PadManipulator
mov     rcx, qword ptr [rdx+88h]
;force function call for type 1 packet
mov     byte ptr [force_sending_ezstate_packet_flag], 1
call    qword ptr [send_PlayerGeneralInfo]
pop     rdx
pop     rcx

jmp     send_throw_packet_injection_return
send_throw_packet_injection ENDP


extern force_sent_anim_packet_to_backstab_injection_return: qword

PUBLIC force_sent_anim_packet_to_backstab_injection
force_sent_anim_packet_to_backstab_injection PROC
;set the ezstate id to backstabing (225) if we're forcing it
cmp     byte ptr [force_sending_ezstate_packet_flag], 1
jne     continue
mov     r15w, 0E1h
mov     byte ptr [force_sending_ezstate_packet_flag], 0

;original code
continue:
mov     rcx, [rsi+230h]
mov     edx, r15d
call    qword ptr [set_ezStateActiveState]

jmp     force_sent_anim_packet_to_backstab_injection_return
force_sent_anim_packet_to_backstab_injection ENDP


_TEXT    ENDS

END
