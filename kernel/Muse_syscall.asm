; local function
global	syscall_sendrecv
global	syscall_print
global	syscall_block

; local data
SYSCALL_VEC	equ	0x80

[SECTION .data]
NR_MESSAGE	equ	0
NR_PRINTX	equ	1
NR_BLOCK	equ	2

[SECTION .text]
syscall_sendrecv:
	mov	eax, NR_MESSAGE
	mov	edx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	ebx, [esp + 12]

	int	SYSCALL_VEC
	ret

syscall_print:
	mov	eax, NR_PRINTX
	mov	edx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	ebx, [esp + 12]

	int	SYSCALL_VEC
	ret

syscall_block:
	mov	eax, NR_BLOCK
	mov	edx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	ebx, [esp + 12]

	int	SYSCALL_VEC
	ret
