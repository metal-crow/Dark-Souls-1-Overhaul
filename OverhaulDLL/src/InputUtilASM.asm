_DATA   SEGMENT

LAB_140e2af07   dq  140e2af07h

_DATA   ENDS

_TEXT    SEGMENT

;hook XINPUT get state call
EXTERN XInput_Get_State_inject_return: qword
EXTERN intercept_xinput_get_state: PROC

PUBLIC XInput_Get_State_inject
XInput_Get_State_inject PROC

jmp    intercept_xinput_get_state

XInput_Get_State_inject ENDP


EXTERN intercept_IDirectInputDevice8GetDeviceState_Keyboard: PROC
EXTERN IDirectInputDevice8GetDeviceState_Keyboard_injection_return: qword

PUBLIC IDirectInputDevice8GetDeviceState_Keyboard_injection
IDirectInputDevice8GetDeviceState_Keyboard_injection PROC

;verify the return value is ok
cmp     eax, 0
jne     exit

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
sub     rsp, 28h

lea     rcx, [rbx+1B0h+8] ;the 1st qword is not keyboard data
call    intercept_IDirectInputDevice8GetDeviceState_Keyboard

add     rsp, 28h
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
exit:
cmp     eax, 8007001Eh
jz      continue
jmp     qword ptr [LAB_140e2af07]
continue:
mov     rcx, [rbx+1B0h]
jmp     IDirectInputDevice8GetDeviceState_Keyboard_injection_return
IDirectInputDevice8GetDeviceState_Keyboard_injection ENDP


EXTERN intercept_IDirectInputDevice8GetDeviceState_DIJOYSTATE2: PROC
EXTERN IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection_return: qword

PUBLIC IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection
IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection PROC

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
sub     rsp, 28h

lea     rcx, [RDI + 5f0h + 8]
call    intercept_IDirectInputDevice8GetDeviceState_DIJOYSTATE2

add     rsp, 28h
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
movaps  xmmword ptr [rsp+30h], xmm6
lea     rdx, [rbx+180h]
movaps  xmmword ptr [rsp+20h], xmm7

jmp     IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection_return
IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection ENDP

_TEXT    ENDS

END
