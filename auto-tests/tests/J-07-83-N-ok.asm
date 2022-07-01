segment	.text._L1
align	4
_L1:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword 1
	lea	eax, [ebp+-4]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	push	dword 2
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	pop	eax
	imul	dword eax, [esp]
	mov	[esp], eax
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	pop	eax
	add	dword [esp], eax
	call	printi
	add	esp, 4
segment	.data
align	4
global	f:object
f:
	dd	_L1
segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword $f
	pop	eax
	push	dword [eax]
	call	f
