_DATA SEGMENT

aUnnamed_1      dq  1414a5f80h
LAB_1401ad106   dq  1401ad106h
FUN_140d7c7c0   dq  140d7c7c0h
LAB_140d696b0   dq  140d696b0h
FUN_140e435d0   dq  140e435d0h
FUN_140cc5600   dq  140cc5600h
LAB_1410826e5   dq  1410826e5h
FUN_1410c4f60   dq  1410c4f60h
LAB_1410ce1ce   dq  1410ce1ceh

_DATA ENDS

_TEXT SEGMENT

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

FUNC_PROLOGUE_LITE macro
	push	r15
	mov		r15, rsp
	and		rsp, -10h
	sub		rsp, 0C0h
	movaps	[rsp + 0B0h], xmm0
	movaps	[rsp + 0A0h], xmm1
	movaps	[rsp + 90h], xmm2
	movaps	[rsp + 80h], xmm3
	movaps	[rsp + 70h], xmm4
	movaps	[rsp + 60h], xmm5
	mov		[rsp + 58h], rax
	mov		[rsp + 50h], rcx
	mov		[rsp + 48h], rdx
	mov		[rsp + 40h], r8
	mov		[rsp + 38h], r9
	mov		[rsp + 30h], r10
	mov		[rsp + 28h], r11
	mov		[rsp + 20h], r15
endm

FUNC_EPILOGUE_LITE macro
	mov		r15, [rsp + 20h]
	mov		r11, [rsp + 28h]
	mov		r10, [rsp + 30h]
	mov		r9, [rsp + 38h]
	mov		r8, [rsp + 40h]
	mov		rdx, [rsp + 48h]
	mov		rcx, [rsp + 50h]
	mov		rax, [rsp + 58h]
	movaps	xmm5, [rsp + 60h]
	movaps	xmm4, [rsp + 70h]
	movaps	xmm3, [rsp + 80h]
	movaps	xmm2, [rsp + 90h]
	movaps	xmm1, [rsp + 0A0h]
	movaps	xmm0, [rsp + 0B0h]
	mov		rsp, r15
	pop		r15
endm

extern char_loaded: byte

extern char_loaded_injection_return: qword

PUBLIC char_loaded_injection
char_loaded_injection PROC

;write out to our global that we're loaded
mov     char_loaded, 1

;original code
mov     [rsp+20h], rbx
push    rsi
push    rdi
push    r14
sub     rsp, 60h
mov     rax, qword ptr [141aab820h]

jmp     char_loaded_injection_return

char_loaded_injection ENDP

extern char_loading_injection_return: qword

PUBLIC char_loading_injection
char_loading_injection PROC

;write out to our global that we're loading
mov     char_loaded, 0

;original code
push    rdi
sub     rsp, 30h
mov     qword ptr [rsp+20h], -2

jmp     char_loading_injection_return

char_loading_injection ENDP


extern gui_hpbar_max_injection_return: qword
extern Gui_HP_bar_UI_ptr: qword

PUBLIC gui_hpbar_max_injection
gui_hpbar_max_injection PROC

;save the pointer to the HP bar ui size
mov     Gui_HP_bar_UI_ptr, rbx
add     Gui_HP_bar_UI_ptr, 504h
;original code
mov     dword ptr [rbx+508h], 44220000h
mov     qword ptr [rbx+50Ch], 43C80000h

jmp     gui_hpbar_max_injection_return

gui_hpbar_max_injection ENDP

extern stop_durability_damage_injection_return: qword
extern stop_durability_damage_original_jump: qword

PUBLIC stop_durability_damage_hook
stop_durability_damage_hook PROC

    cmp dword ptr[r11+14h], r9d                     ; if the new durability value is NOT lower than the current value
    jle originalcode                                ;     we don't need to change the new value since it's the same or large than the current one
    mov r9d, dword ptr[r11+14h]                     ; else: set the new durability value to the current one (freezing it)
    
    originalcode:                                   ; since the long jump overwrites part of the function it is added here
                                                    ; the long jump overwrites the following four instructions
        mov dword ptr [r11+14h], r9d                ; updates the durability value (to new value - which was set to the current value)
        cmp r10d, dword ptr[rcx+20h]                ; instruction that was overwritten
        jge hopper                                  ; this was tricky, since the relative jump was overwritten we calculate the address it would have jumped to (stop_durability_damage_original_jump)
        mov rax, qword ptr[rcx+20h]                 ; last instruciton that was overwritten
        jmp stop_durability_damage_injection_return ; jump back to the original code (right after the long jump)

    hopper:
        jmp stop_durability_damage_original_jump    ; had to use a hopper & jmp as conditional jumps only support relative mode

stop_durability_damage_hook ENDP


EXTERN grab_movemapstep_return: qword
EXTERN grab_movemapstep_value: qword

PUBLIC grab_movemapstep_injection
grab_movemapstep_injection PROC

mov qword ptr [grab_movemapstep_value], rcx
;original code
mov     [rsp+8], rcx
push    rbx
push    rbp
push    rsi
push    rdi
push    r14
sub     rsp, 30h
jmp     grab_movemapstep_return

grab_movemapstep_injection ENDP


extern ReadInputs_allowed: byte
EXTERN Step_PadManipulator_GetInputs_return: qword

PUBLIC Step_PadManipulator_GetInputs_injection
Step_PadManipulator_GetInputs_injection PROC
;custom code to check bool
cmp     ReadInputs_allowed, 0
jnz     do_read
ret
do_read:
;original code
mov     rax, rsp
push    rbp
push    rsi
push    rdi
push    r12
push    r13
push    r14
push    r15
jmp     Step_PadManipulator_GetInputs_return
Step_PadManipulator_GetInputs_injection ENDP


extern GameSuspendCheck_helper: proc

EXTERN PostProcessor_MainLoop_SuspendCheck_return: qword

PUBLIC PostProcessor_MainLoop_SuspendCheck_injection
PostProcessor_MainLoop_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
LEA     R8, [RBP - 59h]
LEA     RDX, [RBP - 31h]
MOV     RCX, RSI
CALL    qword ptr [FUN_140d7c7c0]
jmp     PostProcessor_MainLoop_SuspendCheck_return
PostProcessor_MainLoop_SuspendCheck_injection ENDP


EXTERN FileLoader_MainLoop_SuspendCheck_return: qword

PUBLIC FileLoader_MainLoop_SuspendCheck_injection
FileLoader_MainLoop_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
LEA     RAX, [RBP + 0D0h]
MOV     qword ptr [RSP + 28h], RAX
LEA     RAX, [RBP - 60h]
jmp     FileLoader_MainLoop_SuspendCheck_return
FileLoader_MainLoop_SuspendCheck_injection ENDP


EXTERN FileLoader_MainLoopAlt_SuspendCheck_return: qword

PUBLIC FileLoader_MainLoopAlt_SuspendCheck_injection
FileLoader_MainLoopAlt_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
LEA     RAX, [RBP + 30h]
MOV     qword ptr [RSP + 28h], RAX
LEA     RAX, [RBP - 8]
MOV     qword ptr [RSP + 20h], RAX
jmp     FileLoader_MainLoopAlt_SuspendCheck_return
FileLoader_MainLoopAlt_SuspendCheck_injection ENDP


EXTERN TransferTaskManager_MainLoop_SuspendCheck_return: qword

PUBLIC TransferTaskManager_MainLoop_SuspendCheck_injection
TransferTaskManager_MainLoop_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
MOV     RAX, qword ptr [RBP + 20h]
LEA     RCX, [RBP + 20h]
CALL    qword ptr [RAX + 18h]
TEST    EAX, EAX
JZ      exit
jmp     qword ptr [LAB_140d696b0]
exit:
jmp     TransferTaskManager_MainLoop_SuspendCheck_return
TransferTaskManager_MainLoop_SuspendCheck_injection ENDP


EXTERN EventManager_MainLoop_SuspendCheck_return: qword

PUBLIC EventManager_MainLoop_SuspendCheck_injection
EventManager_MainLoop_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
MOV     RAX, qword ptr [RBX]
MOV     RCX, RBX
CALL    qword ptr [RAX + 20h]
MOVZX   EAX, byte ptr [RBX + 40h]
TEST    AL, AL
jmp     EventManager_MainLoop_SuspendCheck_return
EventManager_MainLoop_SuspendCheck_injection ENDP


EXTERN EventManager_MainLoop2_SuspendCheck_return: qword

PUBLIC EventManager_MainLoop2_SuspendCheck_injection
EventManager_MainLoop2_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
MOV     RBX, qword ptr [RSP + 28h]
MOV     RCX, qword ptr [RSP + 30h]
CMP     RBX, RCX
JNZ     exit
JMP     qword ptr [LAB_1410826e5]
exit:
jmp     EventManager_MainLoop2_SuspendCheck_return
EventManager_MainLoop2_SuspendCheck_injection ENDP


EXTERN SocketChannelManager_MainLoop_SuspendCheck_return: qword

PUBLIC SocketChannelManager_MainLoop_SuspendCheck_injection
SocketChannelManager_MainLoop_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
MOV     qword ptr [RSP + 28h], R15
MOV     RAX, qword ptr [R15]
OR      EDX, 0ffffffffh
MOV     RCX, R15
jmp     SocketChannelManager_MainLoop_SuspendCheck_return
SocketChannelManager_MainLoop_SuspendCheck_injection ENDP


EXTERN SLSessionRunnable_MainLoop_SuspendCheck_return: qword

PUBLIC SLSessionRunnable_MainLoop_SuspendCheck_injection
SLSessionRunnable_MainLoop_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
OR      EDX, 0ffffffffh
LEA     RCX, [RBX + 18h]
CALL    qword ptr [FUN_140e435d0]
MOV     byte ptr [RSP + 30h], 1
jmp     SLSessionRunnable_MainLoop_SuspendCheck_return
SLSessionRunnable_MainLoop_SuspendCheck_injection ENDP


EXTERN SLSessionRunnable_MainLoop2_SuspendCheck_return: qword

PUBLIC SLSessionRunnable_MainLoop2_SuspendCheck_injection
SLSessionRunnable_MainLoop2_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
MOVZX   EAX, byte ptr [RSP + 30h]
TEST    AL,AL
JNZ     continue
jmp     qword ptr [LAB_1410ce1ce]
continue:
MOV     RCX,qword ptr [RBX + 8h]
CALL    qword ptr [FUN_1410c4f60]
jmp     SLSessionRunnable_MainLoop2_SuspendCheck_return
SLSessionRunnable_MainLoop2_SuspendCheck_injection ENDP


EXTERN FrpgTrophyMan_MainLoop_SuspendCheck_return: qword

PUBLIC FrpgTrophyMan_MainLoop_SuspendCheck_injection
FrpgTrophyMan_MainLoop_SuspendCheck_injection PROC

FUNC_PROLOGUE
call    GameSuspendCheck_helper
FUNC_EPILOGUE

;original code
MOV     R15 ,RAX
MOV     R8, qword ptr [R14 + 10h]
OR      EDX, 0ffffffffh
LEA     RCX, [R14 + 10h]
jmp     FrpgTrophyMan_MainLoop_SuspendCheck_return
FrpgTrophyMan_MainLoop_SuspendCheck_injection ENDP



_TEXT    ENDS

END
