_DATA   SEGMENT

LAB_140e2af07   dq  140e2af07h

_DATA   ENDS

_TEXT    SEGMENT

FUNC_PROLOGUE macro
    pushfq 
    push    rax
    mov     rax,rsp
    and     rsp,-10h
    sub     rsp,000002A0h
    fxsave  [rsp+20h]
    mov     [rsp+00000220h],rbx
    mov     [rsp+00000228h],rcx
    mov     [rsp+00000230h],rdx
    mov     [rsp+00000238h],rsi
    mov     [rsp+00000240h],rdi
    mov     [rsp+00000248h],rax
    mov     [rsp+00000250h],rbp
    mov     [rsp+00000258h],r8
    mov     [rsp+00000260h],r9
    mov     [rsp+00000268h],r10
    mov     [rsp+00000270h],r11
    mov     [rsp+00000278h],r12
    mov     [rsp+00000280h],r13
    mov     [rsp+00000288h],r14
    mov     [rsp+00000290h],r15
endm
 
FUNC_EPILOGUE macro
    mov     r15,[rsp+00000290h]
    mov     r14,[rsp+00000288h]
    mov     r13,[rsp+00000280h]
    mov     r12,[rsp+00000278h]
    mov     r11,[rsp+00000270h]
    mov     r10,[rsp+00000268h]
    mov     r9, [rsp+00000260h]
    mov     r8, [rsp+00000258h]
    mov     rbp,[rsp+00000250h]
    mov     rdi,[rsp+00000240h]
    mov     rsi,[rsp+00000238h]
    mov     rdx,[rsp+00000230h]
    mov     rcx,[rsp+00000228h]
    mov     rbx,[rsp+00000220h]
    fxrstor [rsp+20h]
    mov     rsp,[rsp+00000248h]
    pop     rax
    popfq 
endm


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

FUNC_PROLOGUE
lea     rcx, [rbx+1B0h+8] ;the 1st qword is not keyboard data
call    intercept_IDirectInputDevice8GetDeviceState_Keyboard
FUNC_EPILOGUE

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

FUNC_PROLOGUE
lea     rcx, [RDI + 5f0h + 8]
call    intercept_IDirectInputDevice8GetDeviceState_DIJOYSTATE2
FUNC_EPILOGUE

;original code
movaps  xmmword ptr [rsp+30h], xmm6
lea     rdx, [rbx+180h]
movaps  xmmword ptr [rsp+20h], xmm7

jmp     IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection_return

IDirectInputDevice8GetDeviceState_DIJOYSTATE2_injection ENDP

_TEXT    ENDS

END
