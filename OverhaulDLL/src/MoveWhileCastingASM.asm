_DATA SEGMENT

_DATA ENDS

_TEXT SEGMENT

extern c0000_esd_reader_return: qword
extern c0000_esd_data_buffer: qword

PUBLIC c0000_esd_reader_injection
c0000_esd_reader_injection PROC

;original code
mov     r8d, eax
mov     rdx, rsi
;Check that the src pointer we're copying from (R15) contains the "c0000" string
cmp     word ptr [R15+0D0h], 63h ; 'c'
jne     not_c0000
cmp     word ptr [R15+0D2h], 30h ; '0'
jne     not_c0000
cmp     word ptr [R15+0D4h], 30h ; '0'
jne     not_c0000
cmp     word ptr [R15+0D6h], 30h ; '0'
jne     not_c0000
cmp     word ptr [R15+0D8h], 30h ; '0'
jne     not_c0000
mov     qword ptr [c0000_esd_data_buffer], rdx ;save this address
not_c0000:
mov     rcx, r15
call    qword ptr [rbx+18h]
mov     ebx, eax
jmp     c0000_esd_reader_return

c0000_esd_reader_injection ENDP


_TEXT    ENDS

END
