bits 64
global Start:function (Start.End - Start)
extern Main

section .text
Start:
	cli

	mov rsp, stack.bottom
	mov rbp, 0

	call Main
	.End:

section .bss
align 16
stack:
	resb 64 * 1024
	.bottom:
