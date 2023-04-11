;; Constants
mos_getkey:	equ	$00
mos_load:	equ	$01
mos_save:	equ	$02
mos_cd:		equ	$03
mos_dir:	equ	$04
mos_del:	equ	$05
mos_ren:	equ	$06
mos_mkdir:	equ	$07
mos_sysvars:	equ	$08
mos_editline:	equ	$09
mos_fopen:	equ	$0a
mos_fclose:	equ	$0b
mos_fgetc:	equ	$0c
mos_fputc:	equ	$0d
mos_feof:	equ	$0e
mos_geterror:	equ	$0f
mos_oscli:	equ	$10
mos_copy:	equ	$11
mos_getrtc:	equ	$12
mos_setrtc:	equ	$13
mos_setintvec:	equ	$14
mos_uopen:	equ	$15
mos_uclose:	equ	$16
mos_ugetc:	equ	$17
mos_uputc:	equ	$18

;; File modes
fa_read:	equ	$01
fa_write:	equ	$02
fa_exist:	equ	$00
fa_create:	equ	$04
fa_cr_always:	equ	$08
fa_op_always:	equ	$10
fa_append:	equ	$30

;; A - character to print
putc:
	rst.lil $10
	ret

;; HLU - pointer to string
printZ:
	ld bc,0
	xor a
	rst.lil $18
	ret

	macro MOSCALL func
	ld a, func
	rst.lil $08
	endmacro


