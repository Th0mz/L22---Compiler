segment	.bss
align	4
x:
	resb	4
segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword $x
	pop	eax
	push	dword [eax]
	push	dword $x
	pop	eax
	push	dword [eax]
	pop	eax
	sub	dword [esp], eax
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
