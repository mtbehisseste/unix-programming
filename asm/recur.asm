; recur: implement a recursive function

;    r(n) = 0, if n <= 0
;         = 1, if n == 1
;         = 2*r(n-1) + 3*r(n-2), otherwise
   
;    please call r(19) and store the result in RAX
; ======
; ======
; Enter your codes: (type 'done:' when done)

	push 19
	call r
	jmp tmp

; NOTE since the recurrsion will flush the value in register
; should use stack to store the temporary value
r:
	mov rax, [rsp + 8]
	cmp rax, 0
	jle zero
	cmp rax, 1
	je return
	
	; 2*r(n-1)
	sub rax, 1
	push rax
	call r
	mov rcx, 2
	mul rcx
	mov [rsp], rax		   ; store the value of 2*r(n-1) on stack
						   ; can also flush the parameter value which shold be stripped

	; 3*r(n-2)
	mov rax, [rsp + 0x10]  ; restore rax value
	sub rax, 2
	push rax			   ; for parameter of r
	call r
	add rsp, 8             ; strip the parameter value remain on stack
	mov rcx, 3
	mul rcx
	
	; 2*r(n-1) + 3*r(n-2)
	pop rdx				   ; restore the value of 2*r(n-1)
	add rax, rdx
return:
	ret
zero:
	mov rax, 0
	jmp return

tmp:					   ; set a point for jmp and avoid empty label if jmp to `done` directly
	nop
done:
