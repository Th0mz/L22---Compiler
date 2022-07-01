segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword 1
	push	dword 2
	push	dword 3
	pop	eax
	imul	dword eax, [esp]
	mov	[esp], eax
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
