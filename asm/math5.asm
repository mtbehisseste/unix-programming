; math5: 32-bit signed arithmetic
;     var3 = (var1 * -var2) / (var3 - ebx)
;     note: overflowed part should be truncated
; ======
;       var1 @ 0x600000-600004
;       var2 @ 0x600004-600008
;       var3 @ 0x600008-60000c
; ======
; Enter your codes: (type 'done:' when done)

mov eax, [0x600000]
mov ecx, [0x600004]
neg ecx
imul ecx			; result stores in edx:eax

mov ecx, [0x600008]
sub ecx, ebx
xor edx, edx		; strip value in edx
idiv ecx

mov [0x600008], eax
done:
