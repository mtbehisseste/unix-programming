; posneg: test if registers are positive or negative.
;     if ( eax >= 0 ) { var1 = 1 } else { var1 = -1 }
;     if ( ebx >= 0 ) { var2 = 1 } else { var2 = -1 }
;     if ( ecx >= 0 ) { var3 = 1 } else { var3 = -1 }
;     if ( edx >= 0 ) { var4 = 1 } else { var4 = -1 } 
; ======
;       var1 @ 0x600000-600004
;       var2 @ 0x600004-600008
;       var3 @ 0x600008-60000c
;       var4 @ 0x60000c-600010
; ======
; Enter your codes: (type 'done:' when done)

	cmp eax, 0
	mov eax, 1
	jge labela
	sub eax, 2
labela:
	mov [0x600000], eax

	cmp ebx, 0
	mov eax, 1
	jge labelb
	sub eax, 2
labelb:
	mov [0x600004], eax

	cmp ecx, 0
	mov eax, 1
	jge labelc
	sub eax, 2
labelc:
	mov [0x600008], eax

	cmp edx, 0
	mov eax, 1
	jge labeld
	sub eax, 2
labeld:
	mov [0x60000c], eax
done:
