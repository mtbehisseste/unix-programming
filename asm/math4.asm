; math4: 32-bit signed arithmetic
;     var4 = (var1 * -5) / (-var2 % var3)
;     note: overflowed part should be truncated
; ======
;       var1 @ 0x600000-600004
;       var2 @ 0x600004-600008
;       var3 @ 0x600008-60000c
;       var4 @ 0x60000c-600010
; ======
; Enter your codes: (type 'done:' when done)

mov eax, [0x600000]			; var1
mov ebx, 0xfffffffb
imul ebx
mov ebx, eax

mov eax, [0x600004]			; var2
neg eax
idiv dword ptr[0x600008]		; the remainder is in edx
mov ecx, edx

xor edx, edx				; idiv will check the value in edx:eax
mov eax, ebx				; restore eax
idiv ecx				; divide the remainder

movsx rax, eax
mov [0x60000c], eax
done:
