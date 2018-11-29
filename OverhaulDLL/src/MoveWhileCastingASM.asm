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
;WARNING! WARNING! I DON'T KNOW WHY THIS IS A GOOD INDICATOR OF c0000.esd
;Hi future me, this will probably suck
;For some reason, R9==1 is only when it's c0000.esd
cmp     R9, 1
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
