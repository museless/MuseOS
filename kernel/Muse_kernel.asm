%include "asm.inc"
;============================
;	Muse kernel start
;============================

;----------------------------
;	extern data
;----------------------------
extern	prot_main
extern	prot_hand_exception
extern	intr_default
extern	enable_intr
extern	disable_intr
extern	Muse_main

extern	intr_call
extern	pGdt
extern	pIdt
extern	disp_pos
extern	proc_on_view
extern	tss
extern	reenter
extern	sysTab

;----------------------------
;	global data
;----------------------------
global	_start
global	proc_restart
global	system_call

global	divide_error
global	debug_abort
global	nmi
global	break_point
global	overflow
global	bounds_check
global	inval_opcode
global	no_math_fault
global	double_fault
global	inval_tss
global	seg_not_present
global	stack_fault
global	general_prot
global	page_fault
global	math_fpu_fault
global	align_check
global	machine_check
global	simd_fault

global	intr0
global	intr1
global	intr2
global	intr3
global	intr4
global	intr5
global	intr6
global	intr7
global	intr8
global	intr9
global	intr10
global	intr11
global	intr12
global	intr13
global	intr14
global	intr15


[SECTION .bss]
PlaceOfStack	resb	4 * 1024
StackTop:


[SECTION .text]
_start:
	mov	esp, StackTop
	mov	dword [disp_pos], 0
	
	sgdt	[pGdt]
	call	prot_main
	lgdt	[pGdt]

	lidt	[pIdt]

	jmp	SELECTOR_C : csinit
csinit:
	xor	eax, eax
	mov	ax, SELECTOR_TSS
	ltr	ax

	jmp	Muse_main

;----------------------------
; Intr i8259A 0 ~ 15
;----------------------------
; main i8259A
;----------------------------
%macro	i8259A_M	1
	call	proc_save_reg

	in	al, 0x21
	or	al, (1 << %1)
	out	0x21, al

	mov	al, EOI
	out	0x20, al

	sti
	call	[intr_call + 4 * %1]
	cli

	in	al, 0x21
	and	al, ~(1 << %1)
	out	0x21, al

	ret
%endmacro
;----------------------------
ALIGN 16
intr0:
	i8259A_M	0

ALIGN 16
intr1:
	i8259A_M	1

ALIGN 16
intr2:
	i8259A_M	2

ALIGN 16
intr3:
	i8259A_M	3

ALIGN 16
intr4:
	i8259A_M	4

ALIGN 16
intr5:
	i8259A_M	5

ALIGN 16
intr6:
	i8259A_M	6

ALIGN 16
intr7:
	i8259A_M	7

;----------------------------
; slave i8259A
;----------------------------
%macro	i8259A_S	1
	call	proc_save_reg

	in	al, 0xA1
	or	al, (1 << (%1 - 8))
	out	0xA1, al

	mov	al, EOI
	out	0x20, al
	nop
	out	0xA0, al

	sti
	call	[intr_call + 4 * %1]
	cli

	in	al, 0xA1
	and	al, ~(1 << (%1 - 8))
	out	0xA1, al

	ret
%endmacro
;----------------------------
ALIGN 16
intr8:
	i8259A_S	8

ALIGN 16
intr9:
	i8259A_S	9

ALIGN 16
intr10:
	i8259A_S	0xA

ALIGN 16
intr11:
	i8259A_S	0xB

ALIGN 16
intr12:
	i8259A_S	0xC

ALIGN 16
intr13:
	i8259A_S	0xD

ALIGN 16
intr14:
	i8259A_S	0xE

ALIGN 16
intr15:
	i8259A_S	0xF

;----------------------------
; Fault & Trap & Abort
; 0 ~ 19(no include 9 & 15)
;----------------------------
ALIGN 16
divide_error:
	push	0xFFFFFFFF
	push	0
	jmp	exception

ALIGN 16
debug_abort:
	push	0xFFFFFFFF
	push	1
	jmp	exception

ALIGN 16
nmi:
	push	0xFFFFFFFF
	push	2
	jmp	exception

ALIGN 16
break_point:
	push	0xFFFFFFFF
	push	3
	jmp	exception

ALIGN 16
overflow:
	push	0xFFFFFFFF
	push	4
	jmp	exception

ALIGN 16
bounds_check:
	push	0xFFFFFFFF
	push	5
	jmp	exception

ALIGN 16
inval_opcode:
	push	0xFFFFFFFF
	push	6
	jmp	exception

ALIGN 16
no_math_fault:
	push	7
	push	0xFFFFFFFF
	jmp	exception

ALIGN 16
double_fault:
	push	8
	jmp	exception

ALIGN 16
inval_tss:
	push	10
	jmp	exception

ALIGN 16
seg_not_present:
	push	11
	jmp	exception

ALIGN 16
stack_fault:
	push	12
	jmp	exception

ALIGN 16
general_prot:
	push	13
	jmp	exception

ALIGN 16
page_fault:
	push	14
	jmp	exception

ALIGN 16
math_fpu_fault:
	push	0xFFFFFFFF
	push	16
	jmp	exception

ALIGN 16
align_check:
	push	17
	jmp	exception

ALIGN 16
machine_check:
	push	0xFFFFFFFF
	push	18
	jmp	exception

ALIGN 16
simd_fault:
	push	0xFFFFFFFF
	push	19
	jmp	exception

;----------------------------
exception:
	call	prot_hand_exception
	add	esp, 8

	cli
	hlt

;----------------------------
proc_save_reg:
	push	gs
	push	fs
	push	es
	push	ds
	pushad

	mov	esi, edx

	mov	dx, ss
	mov	ds, dx
	mov	es, dx
	mov	fs, dx

	mov	edx, esi
	mov	esi, esp

	inc	dword [reenter]
	cmp	dword [reenter], 0

	jne	.1

	mov	esp, StackTop
	push	proc_restart
	jmp	dword [esi + 48]
.1:
	push	proc_restart_pop
	jmp	dword [esi + 48]

;----------------------------
proc_restart:
	mov	esp, [proc_on_view]
	lldt	[esp + 96]

	lea	eax, [esp + 72]
	mov	dword [tss + 4], eax
proc_restart_pop:
	dec	dword [reenter]

	popad
	pop	ds
	pop	es
	pop	fs
	pop	gs
	add	esp, 4

	iret

;----------------------------
system_call:
	call	proc_save_reg

	sti
	push	esi

	push	dword [proc_on_view]
	push	ebx
	push	ecx
	push	edx

	call	[sysTab + eax * 4]

	add	esp, 4 * 4

	pop	esi
	mov	dword [esi + 28], eax
	cli

	ret
