segment	.text._L1
align	4
_L1:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
segment	.rodata
align	4
_L1_cdk_emitter_internal:
	dq	1.10000
segment	.text
	push	dword $_L1_cdk_emitter_internal
	pop	eax
	push	dword [eax+4]
	push	dword [eax]
	call	printd
	add	esp, 8
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
	push	dword 4
	call	printi
	add	esp, 4
	push	dword 0
	pop	eax
	leave
	ret
extern	printd
extern	printi
