_DATA SEGMENT

sub_140527330   dq  140527330h
label_1402de1b5 dq  1402de1b5h

_DATA ENDS

_TEXT    SEGMENT

EXTERN CalcCorrectGraph_injection_return: qword

PUBLIC CalcCorrectGraph_injection
CalcCorrectGraph_injection PROC

;original code
mov     [rsp+28h], rbx
movaps  xmm6, xmm0
call    qword ptr [sub_140527330]
mov     rdx, [rsp+28h]

;check the returned result isn't null
test    rdx, rdx
jz      funcAbort
jmp     CalcCorrectGraph_injection_return
funcAbort:
jmp     qword ptr [label_1402de1b5]

CalcCorrectGraph_injection ENDP

_TEXT    ENDS

END
