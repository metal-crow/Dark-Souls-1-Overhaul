_DATA   SEGMENT

private_profile_write_inject_return2 dq 1401b6eeeh

_DATA   ENDS

_TEXT    SEGMENT

EXTERN archives_n_savegame_write_inject_return: qword
EXTERN intercept_create_file_w: PROC

PUBLIC archives_n_savegame_write_inject
archives_n_savegame_write_inject PROC

call    intercept_create_file_w
mov     [rsi+60h], rax
cmp     rax, 0FFFFFFFFFFFFFFFFh
jmp     archives_n_savegame_write_inject_return

archives_n_savegame_write_inject ENDP

EXTERN savegame_n_config_write_inject_return: qword

PUBLIC savegame_n_config_write_inject
savegame_n_config_write_inject PROC

xor     edx, edx
call    intercept_create_file_w
mov     rdi, rax
mov     sil, 1
jmp     savegame_n_config_write_inject_return

savegame_n_config_write_inject ENDP

EXTERN private_profile_get1_inject_return: qword
EXTERN intercept_get_private_profile_int_w: PROC

PUBLIC private_profile_get1_inject
private_profile_get1_inject PROC

mov     rdx, [rbx+508h]
mov     rcx, [r14+rax*8]
call    intercept_get_private_profile_int_w
jmp     private_profile_get1_inject_return

private_profile_get1_inject ENDP

EXTERN private_profile_get2_inject_return: qword

PUBLIC private_profile_get2_inject
private_profile_get2_inject PROC

mov     r8d, eax
mov     rdx, r10
mov     rcx, r11
jmp     intercept_get_private_profile_int_w

private_profile_get2_inject ENDP

extern intercept_write_private_profile_section_w: PROC
EXTERN private_profile_write_inject_return: qword

PUBLIC private_profile_write_inject
private_profile_write_inject PROC

mov     rax, [rbx+10h]
mov     rbx, [rsp+48h]
sub     rax, rdx
sar     rax, 1
jnz     valid_check
jmp     private_profile_write_inject_return
valid_check:
mov     rcx, rbp
call    intercept_write_private_profile_section_w
jmp     qword ptr[private_profile_write_inject_return2]

private_profile_write_inject ENDP

_TEXT    ENDS

END
