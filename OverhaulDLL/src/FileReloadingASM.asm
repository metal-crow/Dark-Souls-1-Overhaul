_DATA SEGMENT

sub_140527330   dq  140527330h
label_1402de1b5 dq  1402de1b5h

_DATA ENDS

_TEXT    SEGMENT

EXTERN CalcCorrectGraph_injection_return: qword

PUBLIC CalcCorrectGraph_injection
CalcCorrectGraph_injection PROC
;original code
mov     [rsp+28h], rbx
movaps  xmm6, xmm0
call    qword ptr [sub_140527330]
mov     rdx, [rsp+28h]

;check the returned result isn't null
test    rdx, rdx
jz      funcAbort
jmp     CalcCorrectGraph_injection_return
funcAbort:
jmp     qword ptr [label_1402de1b5]
CalcCorrectGraph_injection ENDP


EXTERN get_ParamResCap_from_ParamMan_injection_return: qword
EXTERN SoloParamRes_curindex: dword

PUBLIC get_ParamResCap_from_ParamMan_injection
get_ParamResCap_from_ParamMan_injection PROC
;original code
cmp     ecx, 27h
jge     abort
test    edx, edx
js      abort
mov     r8, 141D1B360h
mov     r8, qword ptr [r8]
;custom code to change which SoloParamRes we're accessing
mov     edx, SoloParamRes_curindex
jmp     get_ParamResCap_from_ParamMan_injection_return
abort:
xor     eax, eax
ret
get_ParamResCap_from_ParamMan_injection ENDP


EXTERN call_SetHostPlayerIns_offset_injection_return: qword
extern call_SetHostPlayerIns_offset_helper: proc

PUBLIC call_SetHostPlayerIns_offset_injection
call_SetHostPlayerIns_offset_injection PROC

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

mov     rcx, 1 ;lock
call    call_SetHostPlayerIns_offset_helper

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
mov     rax, [rsi]
mov     rcx, rsi
call    qword ptr [rax+150h]
movaps  xmm7, xmmword ptr [rsp+0D0h]

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

mov     rcx, 0 ;unlock
call    call_SetHostPlayerIns_offset_helper

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

jmp     call_SetHostPlayerIns_offset_injection_return
call_SetHostPlayerIns_offset_injection ENDP


extern reload_chrctrl_injection_helper: proc

PUBLIC reload_chrctrl_location1_injection
reload_chrctrl_location1_injection PROC

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

call    reload_chrctrl_injection_helper

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
mov     rbx, [rsp+70h]
add     rsp, 40h
pop     rdi
pop     rsi
pop     rbp
ret
reload_chrctrl_location1_injection ENDP

PUBLIC reload_chrctrl_location2_injection
reload_chrctrl_location2_injection PROC

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

call    reload_chrctrl_injection_helper

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
add     rsp, 50h
pop     rdi
ret
reload_chrctrl_location2_injection ENDP

_TEXT    ENDS

END
