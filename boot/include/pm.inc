;--------------------------------------
;	protect mode's thing
;--------------------------------------

;--------------------------------------
; some data use in gdt idt ldt or gate
;--------------------------------------
DA_32		EQU	4000h	
DA_LIMIT_4K	EQU	8000h	

DA_DPL0		EQU	00h
DA_DPL1		EQU	20h
DA_DPL2		EQU	40h
DA_DPL3		EQU	60h
;--------------------------------------
; gdt type(in byte 5 & bit(0-3) & present 
; in mem bit 7 and sys descriptor segment
;--------------------------------------
DA_DR		EQU	90h	; present only_read data_seg
DA_DRW		EQU	92h	; present read data_seg
DA_DRWA		EQU	93h	; 存在的已访问可读写数据段类型值
DA_C		EQU	98h	; present only_exec code_seg
DA_CR		EQU	9Ah	; present exec & read code_seg
DA_CCO		EQU	9Ch	; 存在的只执行一致代码段属性值
DA_CCOR		EQU	9Eh	; 存在的可执行可读一致代码段属性值
;--------------------------------------
; sys seg desc
;--------------------------------------
DA_LDT		EQU	82h	; ldt
DA_TaskGate	EQU	85h	; task gate
DA_386TSS	EQU	89h	; 386 tss
DA_386CGate	EQU	8Ch	; 386 call gate
DA_386IGate	EQU	8Eh	; 386 interrupt gate
DA_386TGate	EQU	8Fh	; 386 trap gate
;--------------------------------------

;--------------------------------------
; selector attr
;--------------------------------------
SA_RPL0		EQU	0
SA_RPL1		EQU	1
SA_RPL2		EQU	2
SA_RPL3		EQU	3

SA_TIG		EQU	0
SA_TIL		EQU	4
;--------------------------------------

;--------------------------------------
; page attr
;--------------------------------------
PG_P		EQU	1	; Page Present
PG_RWR		EQU	0	; Page not present in mem & read
PG_RWW		EQU	2	; Write & Read
PG_USS		EQU	0	; Supervisor
PG_USU		EQU	4	; User
;--------------------------------------

;--------------------------------------
; Gdt type
;-------------------------------------- 
%macro	Descriptor 3
	dw	%2 & 0xFFFF
	dw	%1 & 0xFFFF
	db	(%1 >> 16) & 0xFF
	dw	((%2 >> 8) & 0x0F00) | (%3 & 0xF0FF)
	db	(%1 >> 24) & 0xFF
%endmacro
;--------------------------------------