_TEXT    SEGMENT

EXTERN mp_zone_changing_injection_return: qword
extern mp_zone_changing_helper: proc

; Assembly code below is injected into the function that changes the the current mp zone
PUBLIC mp_zone_changing_injection
mp_zone_changing_injection PROC

;original code
mov     [rbx+354h], eax ;preventivly write the zone

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

mov     ecx, eax ;the vanilla zone
mov     rdx, rbx ;the current entity
call    mp_zone_changing_helper

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

;original code
mov     rcx, rbx
mov     eax, [rdi+0Ch]
mov     [rbx+358h], eax

jmp     mp_zone_changing_injection_return

mp_zone_changing_injection ENDP

_TEXT    ENDS

END
