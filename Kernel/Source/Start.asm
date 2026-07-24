default rel
bits 64

global Start:function (Start.End - Start)
extern Main

section .text
Start:
	cli

	; Prepare the stack
	mov rsp, stack.bottom

	; Stack traces terminate at rbp=0
	xor rbp, rbp

	call Main
	.End:

section .bss
; The kernel uses a 64k stack
; The stack must be aligned on a 16-byte boundary as per the SysV ABI requirements
align 16
stack: resb 64 * 1024
.bottom:
