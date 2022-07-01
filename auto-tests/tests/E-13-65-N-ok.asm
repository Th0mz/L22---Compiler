segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword 0
	lea	eax, [ebp+-4]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
_L1:
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 29
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	setle	cl
	mov	[esp], ecx
	pop	eax
	cmp	eax, byte 0
	je	near _L2
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 2
	pop	ecx
	pop	eax
	cdq
	idiv	ecx
	push	edx
	push	dword 0
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	sete	cl
	mov	[esp], ecx
	pop	eax
	cmp	eax, byte 0
	je	near _L3
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	call	printi
	add	esp, 4
_L3:
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	push	dword 1
	pop	eax
	add	dword [esp], eax
	push	dword [esp]
	lea	eax, [ebp+-4]
	push	eax
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
