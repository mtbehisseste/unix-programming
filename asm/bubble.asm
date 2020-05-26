; bubble: bubble sort for 10 integers
; ======
;       a[0] @ 0x600000-600004
;       a[1] @ 0x600004-600008
;       a[2] @ 0x600008-60000c
;       a[3] @ 0x60000c-600010
;       a[4] @ 0x600010-600014
;       a[5] @ 0x600014-600018
;       a[6] @ 0x600018-60001c
;       a[7] @ 0x60001c-600020
;       a[8] @ 0x600020-600024
;       a[9] @ 0x600024-600028
; ======
; Enter your codes: (type 'done:' when done)

		mov ecx, 0xa
		mov edi, 0x600000
outer:
		mov edx, 0
		mov eax, [edi]
inner:
		mov ebx, [edi + edx * 4]
		cmp ebx, eax
		jg noswap
		mov [edi], ebx		; swap
		mov [edi + edx * 4], eax
		mov eax, [edi]
noswap:
		inc edx
		cmp edx, ecx
		jne inner

		add edi, 0x4
		loop outer
done:
