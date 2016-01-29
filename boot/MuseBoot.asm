org	0x7c00

jmp	boot_seg

%include "MuseLoad.inc"
;======================================
;some def here
BaseOfStack	equ	0x7c00

TRANS_SECT_NR	equ	2
SECT_BUF_SIZE	equ	TRANS_SECT_NR * 512

disk_address_packet:	db	0x10
			db	0
			db	TRANS_SECT_NR
			db	0
			dw	0
			dw	SUPER_BLK
			dd	0
			dd	0
;======================================

;--------------------------------------
;All things start
boot_seg:
	mov	ax, cs
	mov	es, ax
	mov	ds, ax
	mov	ss, ax
	mov	sp, BaseOfStack

	call	clean_screen
	mov	dh, 0
	call	DispStr

	mov	dword [disk_address_packet + 8], ROOT_BASE + 1	;pass the boot sect
	call	read_sector
	mov	ax, SUPER_BLK
	mov	fs, ax

	mov	word [disk_address_packet + 4], MuseLoadOff
	mov	word [disk_address_packet + 6], MuseLoadSeg

	mov	ax, MuseLoadSeg
	mov	es, ax

	mov	eax, [fs : SB_ROOT_INODE]
	call	get_inode

	add	eax, ROOT_BASE
	mov	dword [disk_address_packet + 8], eax
	call	read_sector

	mov	bx, word [disk_address_packet + 4]
	mov	si, LoaderFileName

	push	bx	

.str_cmp:
	add	bx, [fs : SB_DIR_ENT_FNAME_OFF]
.1:
	lodsb
	cmp	al, [es : bx]
	je	.2
	jmp	.diff
.2:
	cmp	al, 0
	je	.found
	inc	bx
	jmp	.1
.diff:
	pop	bx
	add	bx, [fs : SB_DIR_ENT_SIZE]
	sub	ecx, [fs : SB_DIR_ENT_SIZE]
	jz	.not_found
	
	mov	dx, SECT_BUF_SIZE
	cmp	bx, dx
	jge	.not_found
	
	push	bx
	mov	si, LoaderFileName
	jmp	.str_cmp
.not_found:
	mov	dh, 3
	call	DispStr
	jmp	$
.found:
	mov	dh, 1
	call	DispStr

	pop	bx

	mov	ax, word [disk_address_packet + 6]
	mov	es, ax

	add	bx, [fs : SB_DIR_ENT_INODE_OFF]
	mov	eax, [es : bx]
		
	call	get_inode
	add	eax, ROOT_BASE
	mov	dword [disk_address_packet + 8], eax
load:
	call	read_sector
	
	cmp	ecx, SECT_BUF_SIZE
	jl	.done
	sub	ecx, SECT_BUF_SIZE
	
	add	dword [disk_address_packet + 8], 2
	add	word [disk_address_packet + 4], SECT_BUF_SIZE
	jmp	load
.done:
	jmp	dword MuseLoadSeg : MuseLoadOff


;======================================
;sth like loader name, output message
LoaderFileName		db	"MuseLoader", 0

MessageLen		equ	8
Message0:		db	"Do Boot "
Message1		db	"Ld Found"
Message2		db	"Err!Shut"
Message3		db	"NoMuseLd"
;======================================

;--------------------------------------
DispStr:
	mov	ax, MessageLen
	mul	dh
	add	ax, Message0
	mov	bp, ax
	mov	ax, ds
	mov	es, ax
	mov	cx, MessageLen
	mov	ax, 0x1301
	mov	bx, 0x7
	mov	dl, 0
	
	int	0x10
	ret
;--------------------------------------
clean_screen:
	mov	ax, 0x600
	mov	bx, 0x700
	mov	cx, 0
	mov	dx, 0x184f
	int	0x10
	ret
;--------------------------------------
read_sector:
	xor	ebx, ebx

	mov	ah, 0x42
	mov	dl, 0x80
	mov	si, disk_address_packet
	int	0x13
	
	mov	ax, word [disk_address_packet + 6]
	mov	es, ax
	
	ret
;--------------------------------------
get_inode:
	dec	eax
	mov	bl, [fs : SB_INODE_SIZE]
	mul	bl
	mov	edx, SECT_BUF_SIZE
	sub	edx, [fs : SB_INODE_SIZE]
	cmp	eax, edx
	jg	err
	
	push	eax
	
	mov	ebx, [fs : SB_NR_IMAP_SECTS]
	mov	edx, [fs : SB_NR_SMAP_SECTS]
	lea	eax, [ebx + edx + 2 + ROOT_BASE]
	mov	dword [disk_address_packet + 8], eax
	call	read_sector

	pop	eax

	mov	edx, eax
	add	edx, [fs : SB_INODE_ISIZE_OFF]
	add	edx, MuseLoadOff
	mov	ecx, [es : edx]
	
	mov	edx, eax
	add	edx, [fs : SB_INODE_START_OFF]
	add	edx, MuseLoadOff
	mov	eax, [es : edx]

	ret
;--------------------------------------
err:
	mov	dh, 2
	call	DispStr
	jmp	$
;--------------------------------------

times	510 - ($ - $$)	db	0
dw	0xaa55