segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword 10
	push	dword $f
	pop	eax
	push	dword [eax]
	call	f
	add	esp, 4
	call	printd
	add	esp, 8
	push	dword 2
	push	dword $f
	pop	eax
	push	dword [eax]
	call	f
	add	esp, 4
	call	printd
	add	esp, 8
	pop	eax
	leave
	ret
	push	dword 0
	pop	eax
	leave
	ret
extern	printd
