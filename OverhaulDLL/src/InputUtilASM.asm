_DATA   SEGMENT

GetDeviceStateInjected  db  0

_DATA   ENDS

_TEXT    SEGMENT

;hook XINPUT get state call
EXTERN XInput_Get_State_inject_return: qword
EXTERN intercept_xinput_get_state: PROC

PUBLIC XInput_Get_State_inject
XInput_Get_State_inject PROC

jmp    intercept_xinput_get_state

XInput_Get_State_inject ENDP

;intercept a call to DirectInput8GetDeviceState so we can find it and inject to it
EXTERN gather_DirectInput8GetDeviceState_inject_return: qword
EXTERN gather_DirectInput8GetDeviceState_helper: proc

PUBLIC gather_DirectInput8GetDeviceState_inject
gather_DirectInput8GetDeviceState_inject PROC

;original code
lea     r8, [rbx+1B8h]
mov     edx, 14h
mov     rax, [rcx]

;get the address to inject to and inject
cmp     byte ptr[GetDeviceStateInjected], 0
jne     orig_code

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

call    gather_DirectInput8GetDeviceState_helper
mov     byte ptr[GetDeviceStateInjected], 1

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
orig_code:
call    qword ptr [rax+48h]
jmp     gather_DirectInput8GetDeviceState_inject_return

gather_DirectInput8GetDeviceState_inject ENDP

;hook DirectInput8GetDeviceState call
extern intercept_IDirectInputDevice8GetDeviceState_inject_return: qword
extern intercept_IDirectInputDevice8GetDeviceState: proc

public intercept_IDirectInputDevice8GetDeviceState_inject
intercept_IDirectInputDevice8GetDeviceState_inject PROC

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

mov     rcx, rbp
mov     rdx, rsi
call    intercept_IDirectInputDevice8GetDeviceState

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
mov     rdi, [rsp+48h]
mov     rbp, [rsp+38h]
mov     rsi, [rsp+40h]

jmp     intercept_IDirectInputDevice8GetDeviceState_inject_return

intercept_IDirectInputDevice8GetDeviceState_inject ENDP

_TEXT    ENDS

END
