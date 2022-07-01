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
