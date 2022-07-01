segment	.data
align	4
x:
	dd	1
segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
_L1:
	push	dword $x
	pop	eax
	push	dword [eax]
	push	dword 3
	neg	dword [esp]
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	setge	cl
	mov	[esp], ecx
	pop	eax
	cmp	eax, byte 0
	je	near _L2
	push	dword $x
	pop	eax
	push	dword [eax]
	call	printi
	add	esp, 4
	push	dword $x
	pop	eax
	push	dword [eax]
	push	dword 1
	pop	eax
	sub	dword [esp], eax
	push	dword [esp]
	push	dword $x
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
	jmp	dword _L1
_L2:
	pop	eax
	leave
	ret
	push	dword 0
	pop	eax
	leave
	ret
extern	printi
