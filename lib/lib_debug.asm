global	stop
global	ele_stop
global	get_cr3
global	get_cr2


; stop
stop:
	jmp	$

; ele_stop
ele_stop:
	mov	eax, [esp + 4]
	mov	ebx, [esp + 8]
	jmp	$

; get_cr2
get_cr2:
	mov	eax, cr2
	ret

; get_cr3
get_cr3:
	mov	eax, cr3
	ret