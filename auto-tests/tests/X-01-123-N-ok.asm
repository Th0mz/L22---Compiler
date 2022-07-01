segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword 0
	push	dword $f
	pop	eax
	push	dword [eax]
	call	f
	add	esp, 4
	call	printd
	add	esp, 8
	push	dword $g
	pop	eax
	push	dword [eax]
	push	dword [esp]
	push	dword $f
	pop	ecx
	pop	eax
	mov	[ecx], eax
