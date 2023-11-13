_DATA SEGMENT

sub_140bd35b0   dq  140bd35b0h

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

FUNC_EPILOGUE_NORAX macro
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
    add     rsp, 8
    popfq 
endm

FUNC_EPILOGUE_NOR9 macro
    mov     r15,[rsp+00000290h]
    mov     r14,[rsp+00000288h]
    mov     r13,[rsp+00000280h]
    mov     r12,[rsp+00000278h]
    mov     r11,[rsp+00000270h]
    mov     r10,[rsp+00000268h]
    ;mov     r9, [rsp+00000260h]
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

extern game_write_playerdata_to_flatbuffer_injection_helper: PROC
extern game_write_playerdata_to_flatbuffer_injection_return: qword

PUBLIC game_write_playerdata_to_flatbuffer_injection
game_write_playerdata_to_flatbuffer_injection PROC

FUNC_PROLOGUE
mov     rdx, r9 ;array length
mov     rcx, r8 ;array start
call    game_write_playerdata_to_flatbuffer_injection_helper
mov     r9, rax ;save over the length
FUNC_EPILOGUE_NOR9

;original code
lea     rdx, [rsp+24h]
mov     rcx, rbx
call    qword ptr [sub_140bd35b0]
jmp     game_write_playerdata_to_flatbuffer_injection_return
game_write_playerdata_to_flatbuffer_injection ENDP


extern construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper: PROC
extern construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return: qword

PUBLIC construct_flatbuffer_from_PlayerStatus_MemberFlags_injection
construct_flatbuffer_from_PlayerStatus_MemberFlags_injection PROC

;original code
lea     rbp, [rsp-110h]
sub     rsp, 210h

FUNC_PROLOGUE
call    construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper
FUNC_EPILOGUE

;force arg6 to be always 0 so the code doesn't ignore if the is_present flag is set
mov     r9b, 0

jmp     construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return
construct_flatbuffer_from_PlayerStatus_MemberFlags_injection ENDP


extern finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper: PROC
extern finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return: qword

PUBLIC finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection
finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection PROC

FUNC_PROLOGUE
call    finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_helper
FUNC_EPILOGUE

;original code
mov     rbx, [rsp+268h]
add     rsp, 210h
jmp     finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection_return
finish_construct_flatbuffer_from_PlayerStatus_MemberFlags_injection ENDP


extern set_MemberFlags_bitflag_injection_helper: PROC
extern set_MemberFlags_bitflag_injection_return: qword

PUBLIC set_MemberFlags_bitflag_injection
set_MemberFlags_bitflag_injection PROC

FUNC_PROLOGUE
mov     rcx, r8 ;the flag id
call    set_MemberFlags_bitflag_injection_helper
FUNC_EPILOGUE_NORAX

;check if we allow this flag to be set
cmp     al, 0
jne     exit
;just return from the function immediatly
add     rsp,028h
ret

;original code
exit:
mov     rax, r8
add     r9, 10h
shr     rax, 6
lea     rdx, [rcx+rax*8]
jmp     set_MemberFlags_bitflag_injection_return
set_MemberFlags_bitflag_injection ENDP

_TEXT    ENDS
END
