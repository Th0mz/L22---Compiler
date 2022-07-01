segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword 47
	call	printi
	add	esp, 4
	push	dword 47
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
