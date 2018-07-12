_TEXT SEGMENT

extern main_dead_angle_injection_return: qword
extern main_dead_angle_helper: PROC

PUBLIC main_dead_angle_injection
main_dead_angle_injection PROC

;save temp registers
sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
push    rax
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11

mov     rdx, r15 ;attacker
;mov     rcx, rcx ;target
call    main_dead_angle_helper

pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h
jmp     main_dead_angle_injection_return

main_dead_angle_injection ENDP

_TEXT    ENDS

END
