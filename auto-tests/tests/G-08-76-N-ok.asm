segment	.text._L1
align	4
_L1:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
	pop	eax
	leave
	ret
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
	push	dword 8
	call	printi
	add	esp, 4
	push	dword 0
	pop	eax
	leave
	ret
extern	printi
