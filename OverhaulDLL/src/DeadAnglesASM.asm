_DATA SEGMENT

jnz_one dq 14032930eh
jmp_two dq 140334513h

_DATA ENDS

_TEXT SEGMENT

extern main_dead_angle_injection_return: qword
extern main_dead_angle_helper: PROC

PUBLIC main_dead_angle_injection
main_dead_angle_injection PROC

;save temp registers
sub     rsp, 8
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
push    r9
push    r10
push    r11
sub     rsp, 20h

mov     rdx, r12 ;attacker
mov     rcx, r14 ;target
call    main_dead_angle_helper
cmp     eax, 0
jne     exit
;alter the test codes
mov     byte ptr [rsp+70h+56+64+8+20h], 0 ;account for our register saves in stack offset
mov     bl, 0

exit:
add     rsp, 20h
pop     r11
pop     r10
pop     r9
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
add     rsp, 8

;original code
cmp     [rsp+70h], dil
jz      jmp_case
jmp     qword ptr [jnz_one]
jmp_case:
jmp     qword ptr [jmp_two]

main_dead_angle_injection ENDP

_TEXT    ENDS

END
