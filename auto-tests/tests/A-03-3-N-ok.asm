segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 2048
segment	.rodata
align	4
_L1:
	db	"ABCD", 0
segment	.text
	push	dword $_L1
	call	prints
	add	esp, 4
	pop	eax
	leave
	ret
	push	dword 0
	pop	eax
	leave
	ret
extern	prints
