_DATA SEGMENT

abort_enter dq 14063100fh

_DATA ENDS

_TEXT SEGMENT

extern equipment_change_unlock_injection_return: qword
EXTERN equipment_change_unlock_injection_helper: PROC

PUBLIC equipment_change_unlock_injection
equipment_change_unlock_injection PROC

sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
;push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

mov     ecx, esi ;the menu slot user wants to change
call    equipment_change_unlock_injection_helper

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
;pop     rax
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

test    al, al
jz      no_equip_change ;false, so no change allowed

;original code
mov     qword ptr [rsp+144], rbx
mov     qword ptr [rsp+168], rdi

jmp     equipment_change_unlock_injection_return

no_equip_change:
jmp     qword ptr [abort_enter]

equipment_change_unlock_injection ENDP

_TEXT    ENDS

END
