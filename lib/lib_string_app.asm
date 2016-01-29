;---------------;
; locate data	;
;---------------;
global	memcpy
global	memset
global	strncpy
global	strlen
global	strncmp


; the next function used to do sth memory thing
;-----memcpy-----;
memcpy:
	push	ebp
	mov	ebp, esp
	
	push	esi

	mov	edx, [ebp + 16]	; len
	mov	esi, [ebp + 12]	; src
	mov	edi, [ebp + 8]	; dst
.memcop:
	cmp	edx, 0
	je	.finish
	
	mov	al, byte [esi]
	mov	byte [edi], al
	inc	esi
	inc	edi

	dec	edx
	jmp	.memcop
.finish:
	mov	eax, [ebp + 8]

	pop	esi
	pop	ebp

	ret

;-----memset-----;
memset:
	mov	ecx, [esp + 12]	; len
	mov	eax, [esp + 8]	; char
	mov	edi, [esp + 4]	; dest
.loop
	mov	byte [edi], al
	inc	edi

	loop	.loop
.fin:
	ret

;-----strncpy-----;
strncpy:
	mov	ecx, [esp + 12]	;len
	mov	esi, [esp + 8]	;src
	mov	edi, [esp + 4]	;dst

.loop:
	mov	al, [esi]
	inc	esi

	mov	[edi], al
	inc	edi

	loop	.loop

.finish:
	mov	al, 0
	mov	[edi], al

	mov	eax, [esp + 8]

	ret


;-----strlen-----;
strlen:
	mov	edi, [esp + 4]
	xor	eax, eax
.loop:
	cmp	byte [edi], 0
	je	.finish

	inc	eax
	inc	edi
	jmp	.loop

.finish:
	ret


;-----strncmp-----;
strncmp:
	mov	ecx, [esp + 12]	;len
	mov	esi, [esp + 8]	;cmp1
	mov	edi, [esp + 4]	;cmp2

	xor	eax, eax
	mov	al, [esi]

.cir:
	cmp	al, 0
	je	.finish

	cmp	al, [edi]
	jne	.finish

	inc	esi
	inc	edi
	mov	al, [esi]

	loop	.cir

.finish:
	sub	al, [edi]
	ret
	

	