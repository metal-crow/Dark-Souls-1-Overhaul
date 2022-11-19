_DATA   SEGMENT

_DATA   ENDS

_TEXT    SEGMENT

EXTERN custom_strings_return: qword
extern custom_header_load_msg_address: qword
extern custom_buttons_load_msg_address: qword
extern custom_buttons_load_alt_msg_address: qword
extern custom_header_delete_msg_address: qword
extern custom_buttons_delete_msg_address: qword

; Injected assembly function that enables custom messages in place of "Select data to load" and "Select data to delete"
PUBLIC custom_strings_inject
custom_strings_inject PROC

cmp     rdi, 0A028h  ; 41000 = ID of "Select data to load"
je      string_id_load
cmp     rdi, 0A02Ah  ; 41002 = ID of "Select data to load" (alt)
je      string_id_load_alt
cmp     rdi, 0A029h  ; 41001 = ID of "Select data to delete"
je      string_id_delete
jmp     original_code

string_id_load:
cmp     byte ptr[rdx], 3Ch ; 0x3C == '<'; First character of buttons/dialog strings but not header string (language-independent implementation)
jne     display_custom_load_string
cmp     byte ptr[rdx + 6h], 65h ; 0x65 == 'e'; Fourth character of buttons string (and not dialog string)
je      display_custom_load_buttons_string
jmp     original_code

string_id_load_alt:
cmp     byte ptr[rdx], 3Ch
jne     display_custom_load_string
cmp     byte ptr[rdx + 6h], 65h
je      display_custom_load_buttons_string_alt
jmp     original_code

string_id_delete:
cmp     byte ptr[rdx], 3Ch
jne     display_custom_delete_string
cmp     byte ptr[rdx + 6h], 65h
je      display_custom_delete_buttons_string
jmp     original_code

; Display custom message in place of "Select data to load"
display_custom_load_string:
mov     rdx, custom_header_load_msg_address
jmp     original_code

; Display custom message in place of buttons string
display_custom_load_buttons_string:
mov     rdx, custom_buttons_load_msg_address
jmp     original_code

; Display custom message in place of buttons string
display_custom_load_buttons_string_alt:
mov     rdx, custom_buttons_load_alt_msg_address
jmp     original_code

; Display custom message in place of "Select data to delete"
display_custom_delete_string:
mov     rdx, custom_header_delete_msg_address
jmp     original_code

; Display custom message in place of buttons string
display_custom_delete_buttons_string:
mov     rdx, custom_buttons_delete_msg_address

original_code:
mov     rdi, r8
mov     [rsp+20h], r8
mov     rax, rdx
mov     [rsp+38h], rdx
jmp     custom_strings_return

custom_strings_inject ENDP

_TEXT    ENDS

END
