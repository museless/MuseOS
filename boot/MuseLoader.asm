org	0x100

jmp	LABEL_LOADER

%include "MuseLoad.inc"
%include "pm.inc"
;======================================
;some def here
BaseOfStack	equ	0x100

TRANS_SECT_NR	equ	2
SECT_BUF_SIZE	equ	TRANS_SECT_NR * 512

disk_address_packet:	db	0x10
			db	0
			db	TRANS_SECT_NR
			db	0
			dw	MuseKerOff
			dw	MuseKerSeg
			dd	0
			dd	0
;======================================


[section .gdt]
LABEL_GDT:	Descriptor	 0,	  0, 0
LABEL_DESC_C	Descriptor	 0, 0xfffff, DA_32 | DA_CR | DA_LIMIT_4K
LABEL_DESC_D	Descriptor	 0, 0xfffff, DA_32 | DA_DRW | DA_LIMIT_4K
LABEL_DESC_V	Descriptor 0xb8000,  0xffff, DA_DRW | DA_DPL3

GdtLen		equ	$ - LABEL_GDT
GdtPtr		dw	GdtLen - 1
		dd	MuseLoadPhyAddr + LABEL_GDT

SELECT_DESC_C	EQU	LABEL_DESC_C - LABEL_GDT
SELECT_DESC_D	EQU	LABEL_DESC_D - LABEL_GDT
SELECT_DESC_V	EQU	LABEL_DESC_V - LABEL_GDT + SA_RPL3


;--------------------------------------
err:
	mov	dh, 2
	call	DispStrInReal
	jmp	$
;--------------------------------------
[section .text]
LABEL_LOADER:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack
	
	mov	dh, 0
	call	DispStrInReal

	mov	ebx, 0
	mov	di, _MemBuf
get_mem:
	mov	edx, 0x0534D4150
	mov	ecx, 20
	mov	eax, 0xe820
	int	0x15
	jc	mem_chk_fail
	add	di, 20
	inc	dword [_ARDSNumber]
	
	cmp	ebx, 0
	jne	get_mem
	jmp	mem_chk_succ
mem_chk_fail:
	mov	dh, 4
	call	DispStrInReal
	jmp	$
mem_chk_succ:
	mov	eax, [fs : SB_ROOT_INODE]
	call	get_inode

	add	eax, ROOT_BASE
	mov	dword [disk_address_packet + 8], eax

	call	read_sector
	
	mov	bx, word [disk_address_packet + 4]
	mov	si, KerFileName
	
	push	bx

.str_cmp:
	add	bx, [fs : SB_DIR_ENT_FNAME_OFF]
.1:
	lodsb
	cmp	al, [es : bx]
	jne	.diff

	cmp	al, 0
	je	.found
	inc	bx
	jmp	.1
.diff:
	pop	bx
	
	add	bx, [fs : SB_DIR_ENT_SIZE]
	sub	ecx, [fs : SB_DIR_ENT_SIZE]
	jz	.notfound

	cmp	bx, SECT_BUF_SIZE

	jge	err

	push	bx
	mov	si, KerFileName
	jmp	.str_cmp
.notfound:
	mov	dh, 1
	call	DispStrInReal
	jmp	$
.found:
	pop	bx
	
	add	bx, [fs : SB_DIR_ENT_INODE_OFF]
	mov	eax,[es : bx]
	call	get_inode
	
	add	eax, ROOT_BASE
	mov	dword [disk_address_packet + 8], eax
load:
	call	read_sector
	cmp	ecx, SECT_BUF_SIZE
	jl	.done
	sub	ecx, SECT_BUF_SIZE
	
	add	dword [disk_address_packet + 8], 2
	cmp	word [disk_address_packet + 4], 0xFC00
	je	.again
	add	word [disk_address_packet + 4], SECT_BUF_SIZE
	jmp	load
.again:
	mov	word [disk_address_packet + 4], 0
	add	word [disk_address_packet + 6], 0x100
	jmp	load
.done:
	mov	dh, 1
	call	DispStrInReal
	
	lgdt	[GdtPtr]
	cli

	in	al, 92h
	or	al, 00000010b
	out	92h, al

	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	jmp	dword SELECT_DESC_C : (MuseLoadPhyAddr + LABEL_PROTECT)

;--------------------------------------
get_inode:
	dec	eax
	mov	dl, [fs : SB_INODE_SIZE]
	mul	dl
	mov	edx, SECT_BUF_SIZE
	sub	edx, [fs : SB_INODE_SIZE]
	cmp	eax, edx
	jge	err

	push	eax
	
	mov	ebx, dword [fs : SB_NR_IMAP_SECTS]
	mov	edx, dword [fs : SB_NR_SMAP_SECTS]
	lea	eax, [ebx + edx + 2 + ROOT_BASE]
	mov	dword [disk_address_packet + 8], eax
	call	read_sector
	
	pop	eax

	mov	edx, eax
	add	edx, MuseKerOff
	add	edx, [fs : SB_INODE_ISIZE_OFF]
	mov	ecx, [es : edx]
	
	mov	edx, eax
	add	edx, MuseKerOff
	add	edx, [fs : SB_INODE_START_OFF]
	mov	eax, [es : edx]
	
	ret
;---------------------------------------
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
DispStrInReal:
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
	add	dh, 3
	int	0x10
	ret
;--------------------------------------


;======================================
; sth like loader name, output message
KerFileName	db	"MuseKernel", 0

MessageLen	equ	11
Message0	db	"Loading now"
Message1	db	"Kern Found "
Message2	db	"Err !! Shut"
Message3	db	"No MuseKern"
Message4	db	"ChkMem Fail"
;======================================

;--------------------------------------
; start the protect mode
;--------------------------------------
[section .code32]
ALIGN 32
[BITS 32]
LABEL_PROTECT:
	mov	ax, SELECT_DESC_D
	mov	es, ax
	mov	ss, ax
	mov	ds, ax
	mov	fs, ax
	mov	ax, SELECT_DESC_V
	mov	gs, ax
	mov	esp, TopOfStack

	call	DispMemInfo
	
	call	Initx87
	call	DispReturn

	push	sSLine
	call	DispStr
	add	esp, 4

	call	LoadData
	call	LoadPaging
	call	InitKernel

	jmp	dword SELECT_DESC_C : MuseKerEntry

;--------------------------------------
InitKernel:
	mov	ebx, MuseKerPhyAddr
	add	ebx, MuseKerOff
	mov	cx, word [ebx + 0x2C]	; phnum
	movzx	ecx, cx
	mov	esi, dword [ebx + 0x1C]	; entry
	add	esi, ebx
	
.copy:
	mov	eax, [esi]
	cmp	eax, 0
	je	.type	; elf's program header's p_type

	push	ebx
	add	ebx, dword [esi + 0x4]
	
	push	dword [esi + 0x10]	; len
	push	ebx			; src
	push	dword [esi + 0x8]	; dst
	call	MemCpy
	add	esp, 12

	pop	ebx
.type:
	add	esi, 0x20
	dec	ecx
	jnz	.copy
	
	ret
;--------------------------------------
Initx87:
	mov	eax, cr0
	or	eax, 2
	mov	cr0, eax
.1:
	fninit
	fstsw	ax
	cmp	ax, 0
	je	.2
	
	push	sFpuHasn
	call	DispStr
	add	esp, 4
	
	mov	eax, cr0
	xor	eax, 6
	mov	cr0, eax
	ret
.2:
	fsetpm

	push	sFpuHas
	call	DispStr
	add	esp, 4
	
	ret
;--------------------------------------
LoadPaging:
	xor	edi, edi
	xor	ecx, ecx

	mov	eax, [MemSize]
	mov	ebx, 0x400000
	div	ebx
	mov	ecx, eax
	cmp	edx, 0
	je	.1
	inc	ecx
	mov	dword [NrPageTbl], ecx
.1:
	mov	edi, PageDirBase
	xor	eax, eax
	mov	eax, PageTblBase | PG_P | PG_RWW | PG_USU
.2:
	stosd
	add	eax, 4096
	loop	.2

	mov	ecx, dword [NrPageTbl]
	mov	eax, 1024
	mul	ecx
	mov	ecx, eax
	xor	eax, eax
	mov	eax, PG_P | PG_RWW | PG_USU
	mov	edi, PageTblBase
.3:
	stosd
	add	eax, 4096
	loop	.3

	mov	eax, PageDirBase
	mov	cr3, eax

	mov	eax, cr0
	or	eax, 0x80000000
	mov	cr0, eax
	
	nop
	nop

	ret
;--------------------------------------
LoadData:
	mov	edi, ELEMENT_ADDR

	mov	eax, [MemSize]
	mov	dword [es : edi], eax

	mov	eax, [NrPageTbl]
	mov	dword [es : edi + 4], eax

	ret
;--------------------------------------
DispMemInfo:
	mov	edx, dword [ARDSNumber]
	mov	esi, MemBuf
	mov	edi, esi
.do:
	mov	ecx, 5
.loop:
	loop	.loop

	cmp	dword [es : edi + 0x10], 0x1
	jne	.1
	mov	eax, dword [es : edi]
	add	eax, dword [es : edi + 8]
	cmp	dword [MemSize], eax
	jg	.1
	mov	dword [MemSize], eax
.1:
	add	edi, 20
	dec	edx
	cmp	edx, 0
	jne	.do
.2:
	push	sMemLine
	call	DispStr
	add	esp, 4
	
	push	dword [MemSize]
	call	DispInt
	add	esp, 4
	
	call	DispReturn

	ret

;--------------------------------------
DispInt:
	mov	eax, [esp + 4]
	shr	eax, 24
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 16
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 8
	call	DispAL

	mov	eax, [esp + 4]
	call	DispAL

	mov	ah, 07h			
	mov	al, 'h'
	push	edi
	mov	edi, [DisPos]
	mov	[gs : edi], ax
	add	edi, 4
	mov	[DisPos], edi
	pop	edi

	ret
;--------------------------------------
DispAL:
	push	ecx
	push	edx
	push	edi

	mov	edi, [DisPos]

	mov	ah, 0Fh			
	mov	dl, al
	shr	al, 4
	mov	ecx, 2
.begin:
	and	al, 01111b
	cmp	al, 9
	ja	.1
	add	al, '0'
	jmp	.2
.1:
	sub	al, 0Ah
	add	al, 'A'
.2:
	mov	[gs : edi], ax
	add	edi, 2

	mov	al, dl
	loop	.begin
	;add	edi, 2

	mov	[DisPos], edi

	pop	edi
	pop	edx
	pop	ecx

	ret
;--------------------------------------
DispReturn:
	push	edx
	push	esi
	push	edi

	push	sReturn
	call	DispStr			
	add	esp, 4
	
	pop	edi
	pop	esi
	pop	edx

	ret
;--------------------------------------
DispStr:
	push	ebp
	mov	ebp, esp
	
	mov	esi, [ebp + 8]
	mov	edi, [DisPos]
.1:
	cmp	byte [ds : esi], 0
	je	.fin
	cmp	byte [ds : esi], 0xA
	jne	.2
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0xFF
	inc	al
	mul	bl
	mov	edi, eax
	jmp	.3
.2:
	mov	al, byte [ds : esi]

	mov	ah, 0x3
	mov	word [gs : edi], ax
	add	edi, 2
.3:
	inc	esi
	jmp	.1
.fin:
	mov	dword [DisPos], edi
	mov	al, byte [ds : esi]	

	pop	ebp
	ret
;--------------------------------------
MemCpy:
	push	ebp
	mov	ebp, esp
	
	push	esi

	mov	edx, [ebp + 16]	; len
	mov	esi, [ebp + 12]	; src
	mov	edi, [ebp + 8]	; dst
.memcop:
	cmp	edx, 0
	je	.finish
	
	mov	al, byte [ds : esi]
	mov	byte [es : edi], al
	inc	esi
	inc	edi

	dec	edx
	jmp	.memcop
.finish:
	pop	esi
	pop	ebp

	ret
;--------------------------------------


;======================================
; data used between in prot & real
;======================================
[SECTION .data]
ALIGN	32
LABEL_DATA:
; used in the read_mode
_MemSize	dd	0
_DisPos		dd	(80 * 6 + 0) * 2
_NrPageTbl	dd	0
_ARDSNumber	dd	0
_sReturn	db	0xa, 0
_sMemLine	db	"Ram size:", 0
_sFpuHas	db	"Fpu Init Ok!", 0xa, 0
_sFpuHasn	db	"Not Fpu", 0xa, 0
_sSLine		db	"----------------", 0xa, 0
_MemBuf		times	512	db	0

_ARDStruct:
	_BaseAddrLow	dd	0
	_BaseAddrHigh	dd	0
	_LengthLow	dd	0
	_LengthHigh	dd	0
	_Type		dd	0


; used in the protect mode
MemSize		equ	MuseLoadPhyAddr + _MemSize
DisPos		equ	MuseLoadPhyAddr	+ _DisPos
NrPageTbl	equ	MuseLoadPhyAddr + _NrPageTbl
ARDSNumber	equ	MuseLoadPhyAddr + _ARDSNumber
sReturn		equ	MuseLoadPhyAddr + _sReturn
sMemLine	equ	MuseLoadPhyAddr + _sMemLine
sFpuHas		equ	MuseLoadPhyAddr + _sFpuHas
sFpuHasn	equ	MuseLoadPhyAddr + _sFpuHasn
sSLine		equ	MuseLoadPhyAddr	+ _sSLine
MemBuf		equ	MuseLoadPhyAddr + _MemBuf

ADRStruct:
	BaseAddrLow	equ	MuseLoadPhyAddr + _BaseAddrLow
	BaseAddrHigh	equ	MuseLoadPhyAddr + _BaseAddrHigh
	LengthLow	equ	MuseLoadPhyAddr + _LengthLow
	LengthHigh	equ	MuseLoadPhyAddr + _LengthHigh
	Type		equ	MuseLoadPhyAddr + _Type
;--------------------------------------

SpaceOfStack	times	1000h	db	0
TopOfStack	equ	MuseLoadPhyAddr + $
;======================================