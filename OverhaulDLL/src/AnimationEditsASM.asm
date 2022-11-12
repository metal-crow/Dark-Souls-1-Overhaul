_DATA SEGMENT

sub_1404363c0   dq  1404363c0h

_DATA ENDS

_TEXT    SEGMENT

EXTERN read_body_aid_injection_helper_function: PROC
EXTERN animation_entry_set_return: qword

PUBLIC animation_entry_set_injection
animation_entry_set_injection PROC

;original code
mov     dword ptr [rax+5Ch], 3F800000h
mov     rax, [rbx+8]
mov     [rax+10h], r14d

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

add     rax, 5Ch
mov     rdx, rax ;Animation entry speed ptr
mov     rcx, rdi ;Animation aid ptr
call    read_body_aid_injection_helper_function

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
jmp animation_entry_set_return

animation_entry_set_injection ENDP

EXTERN disable_whiff_animations_injection_helper: PROC
EXTERN disable_whiff_animations_injection_return: qword

PUBLIC disable_whiff_animations_injection
disable_whiff_animations_injection PROC

;original code
mov     esi, [rdi+80h]
add     esi, 28h
mov     edx, esi
call    qword ptr [sub_1404363c0]

;don't save/restore rax since it's the return val
sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
push    rcx
push    rdx
push    r8
push    r9
push    r10
push    r11
sub     rsp, 20h

mov     cl, al ;pass in the value that determines if we whiff as the arg
call    disable_whiff_animations_injection_helper

add     rsp, 20h
pop     r11
pop     r10
pop     r9
pop     r8
pop     rdx
pop     rcx
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

jmp     disable_whiff_animations_injection_return

disable_whiff_animations_injection ENDP


_TEXT    ENDS

END
