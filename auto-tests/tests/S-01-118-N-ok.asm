segment	.bss
align	4
f:
	resb	4
segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
segment	.text._L1
align	4
_L1:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	push	dword 3
	lea	eax, [ebp+-4]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	lea	eax, [ebp+-4]
	push	eax
	pop	eax
	push	dword [eax]
	call	printi
	add	esp, 4
	push	dword [esp]
	push	dword $f
	pop	ecx
	pop	eax
	mov	[ecx], eax
