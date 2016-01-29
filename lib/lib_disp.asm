;----------------------------
; all of the next function 
; just used before the printx
; or printf not finished
;----------------------------

;----------------------------
; local function 
;----------------------------
global	disp_str
global	disp_color_str
global	disp_int

;----------------------------
; extern data
;----------------------------
extern	disp_pos


;----------------------------
disp_str:
	push	ebp
	mov	ebp, esp
	
	mov	esi, dword [ebp + 8]
	mov	edi, dword [disp_pos]
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
	mov	dword [disp_pos], edi
	mov	al, byte [esi]

	pop	ebp
	ret
;----------------------------
disp_color_str:
	push	ebp
	mov	ebp, esp
	
	mov	esi, [ebp + 8]
	mov	ecx, [ebp + 12] ; color
	mov	edi, dword [disp_pos]
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

	mov	ah, cl
	mov	word [gs : edi], ax
	add	edi, 2
.3:
	inc	esi
	jmp	.1
.fin:
	mov	dword [disp_pos], edi
	mov	al, byte [esi]	

	pop	ebp
	ret
;----------------------------
disp_int:
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

	mov	ah, 0x17			
	mov	al, 'h'
	push	edi
	mov	edi, [disp_pos]
	mov	[gs : edi], ax
	add	edi, 4
	mov	[disp_pos], edi
	pop	edi

	ret
;----------------------------
DispAL:
	push	ecx
	push	edx
	push	edi

	mov	edi, [disp_pos]

	mov	ah, 0x1C	; blue background, light red word	
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

	mov	[disp_pos], edi

	pop	edi
	pop	edx
	pop	ecx

	ret
;----------------------------
