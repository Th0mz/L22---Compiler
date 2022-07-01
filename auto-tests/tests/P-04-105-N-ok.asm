segment	.bss
align	4
f:
	resb	4
segment	.data
align	4
d:
	dq	4.00000
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
	push	dword 0
	lea	eax, [ebp+-8]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	push	dword 7
	push	dword 3
	pop	ecx
	sal	dword [esp], cl
	pop	eax
	sub	esp, eax
	push	esp
	push	dword [esp]
	push	dword $f
	pop	ecx
	pop	eax
	mov	[ecx], eax
