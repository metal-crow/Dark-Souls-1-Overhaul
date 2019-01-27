_DATA   SEGMENT

_DATA   ENDS

_TEXT    SEGMENT

EXTERN XInput_Get_State_inject_return: qword
EXTERN intercept_xinput_get_state: PROC

PUBLIC XInput_Get_State_inject
XInput_Get_State_inject PROC

jmp    intercept_xinput_get_state

XInput_Get_State_inject ENDP

_TEXT    ENDS

END
