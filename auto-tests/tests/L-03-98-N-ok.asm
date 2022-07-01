segment	.bss
align	4
g:
	resb	4
segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword 3
	push	dword $f
	pop	eax
	push	dword [eax]
	call	f
	add	esp, 4
	call	printi
	add	esp, 4
	pop	eax
	leave
	ret
	push	dword 0
	pop	eax
	leave
	ret
extern	printi
