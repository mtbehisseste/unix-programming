; loop15:
;     str1 is a string contains 15 lowercase and uppercase alphbets.
;     implement a loop to convert all alplabets to lowercase,
;     and store the result in str2.
; ======
;       str1 @ 0x600000-600010
;       str2 @ 0x600010-600020
; ======
; Enter your codes: (type 'done:' when done)

	mov ecx, 0xf
func:
	mov ebx, 0x60000f
	sub ebx, ecx
	mov al, [ebx]
	or al, 0x20
	add ebx, 0x10
	mov [ebx], al
	loop func
done:
