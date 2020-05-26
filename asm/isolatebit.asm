; isolatebit:
;     get the value bit-11 ~ bit-5 in AX and store the result in val1
;     (zero-based bit index)
; ======
;       val1 @ 0x600000-600001
;       val2 @ 0x600001-600002
; ======
; Enter your codes: (type 'done:' when done)

shr ax, 5
and al, 0x7f
mov [0x600000], al
