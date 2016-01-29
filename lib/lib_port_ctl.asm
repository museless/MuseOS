%include "asm.inc"

;----------------------------
;	local data
;----------------------------
global	out_byte
global	in_byte
global	port_read
global	port_write
global	disable_intr
global	enable_intr
global	open_int
global	close_int

global	set_cr0_wp

;----------------------------
; 	extern data
;----------------------------


;----------------------------
;	out_byte
;----------------------------
; out byte with delay
;----------------------------
out_byte:
	mov	dx, [esp + 4]
	mov	al, [esp + 8]
	mov	di, [esp + 12] ;type

	out	dx, al
	cmp	di, 1
	je	.delay
	jmp	.finish
.delay
	call	io_delay
.finish
	ret

;----------------------------
;	in_byte
;----------------------------
in_byte:
	mov	dx, [esp + 4]
	xor	eax, eax

	in	al, dx

	ret

;----------------------------
;	io_delay
;----------------------------
io_delay:
	nop
	nop
	nop

	ret

;----------------------------
;	port_read
;----------------------------
port_read:
	mov	edx, [esp + 4]
	mov	edi, [esp + 8]
	mov	ecx, [esp + 12]
	shr	ecx, 1

	cld
	rep	insw
	ret

;----------------------------
;	port_write
;----------------------------
port_write:
	mov	edx, [esp + 4]
	mov	esi, [esp + 8]
	mov	ecx, [esp + 12]
	shr	ecx, 1
	
	cld
	rep	outsw
	ret

;----------------------------
;	disable_intr
;----------------------------
disable_intr:
	mov	ecx, [esp + 4]
	pushf
	cli

	mov	ah, 1
	rol	ah, cl
	cmp	cl, 8
	jg	.disable_intr8
.disable_intr0:
	in	al, 0x21
	mov	ch, al
	and	ch, ah
	cmp	ch, 0
	je	.disable_already
	or	al, ah
	out	0x21, al
	jmp	.disable_already

.disable_intr8:
	in	al, 0xA1
	mov	ch, al
	and	ch, ah
	cmp	ch, 0
	je	.disable_already
	or	al, ah
	out	0xA1, al

.disable_already:
	popf

	ret

;----------------------------
;	enable_intr
;----------------------------
enable_intr:
	mov	ecx, [esp + 4]
	cli
	pushf

	mov	ah, ~1
	rol	ah, cl
	cmp	cl, 8
	jg	.enable_intr8
.enable_intr0:
	in	al, 0x21
	and	al, ah
	out	0x21, al
	jmp	.enable_done

.enable_intr8:
	in	al, 0xA1
	and	al, ah
	out	0xA1, al

.enable_done:
	popf
	ret

;----------------------------
;	open_int
;----------------------------
open_int:
	sti
	ret

;----------------------------
;	close_int
;----------------------------
close_int:
	cli
	ret

;----------------------------
;	set_cr0_wp
;----------------------------
set_cr0_wp:
	mov	eax, cr0
	or	eax, 0x00010000
	mov	cr0, eax

	ret