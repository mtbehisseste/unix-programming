; dispbin:
;     given a number in AX, store the corresponding bit string in str1.
;     for example, if AX = 0x1234, the result should be:
;     str1 = 0001001000111000
; ======
;       str1 @ 0x600000-600014
; ======
; Enter your codes: (type 'done:' when done)

mov ecx, 0x10
mov edi, 0x60000f
func:
	mov bl, al
	and bl, 0x1
	
	test bl, bl
	mov dl, 0x30
	jz label
	inc dl
label:
	mov [edi], dl

	dec edi
	shr ax, 1
	loop func
done:
