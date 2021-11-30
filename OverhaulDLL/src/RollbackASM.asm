_DATA SEGMENT

_DATA ENDS

_TEXT    SEGMENT

EXTERN sendType1NetMessage_injection_return: qword
PUBLIC sendType1NetMessage_injection
sendType1NetMessage_injection PROC
;skip original code, just leave this function now
ret
sendType1NetMessage_injection ENDP

EXTERN recvType1NetMessage_injection_return: qword
PUBLIC recvType1NetMessage_injection
recvType1NetMessage_injection PROC
;skip original code, just leave this function now
ret
recvType1NetMessage_injection ENDP

_TEXT    ENDS

END
