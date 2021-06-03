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


EXTERN get_ParamResCap_from_ParamMan_injection_return: qword
EXTERN SoloParamRes_curindex: dword

PUBLIC get_ParamResCap_from_ParamMan_injection
get_ParamResCap_from_ParamMan_injection PROC
;original code
cmp     ecx, 27h
jge     abort
test    edx, edx
js      abort
mov     r8, 141D1B360h
mov     r8, qword ptr [r8]
;custom code to change which SoloParamRes we're accessing
mov     edx, SoloParamRes_curindex
jmp     get_ParamResCap_from_ParamMan_injection_return
abort:
xor     eax, eax
ret
get_ParamResCap_from_ParamMan_injection ENDP

_TEXT    ENDS

END
