segment	.text._L1
align	4
_L1:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
segment	.rodata
align	4
_L2:
	db	"ola", 0
segment	.text
	push	dword $_L2
	call	prints
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
