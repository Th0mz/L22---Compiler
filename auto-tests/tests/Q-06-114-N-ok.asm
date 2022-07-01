segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
segment	.rodata
align	4
_L1_cdk_emitter_internal:
	dq	61.0000
segment	.text
	push	dword $_L1_cdk_emitter_internal
	pop	eax
	push	dword [eax+4]
	push	dword [eax]
	lea	eax, [ebp+-8]
	push	eax
	pop	ecx
	pop	eax
	mov	[ecx], eax
	pop	eax
	mov	[ecx+4], eax
	lea	eax, [ebp+-8]
	push	eax
	push	dword $main
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
