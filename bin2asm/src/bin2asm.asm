MAX_ARGS:	equ	2
	include "starcrt.inc"
	include "mos.inc"

input_name: equ argv
output_name: equ argv+3

banner:
	db "bin2asm (c) 2023 Nihirash",13,10,0
usage:
	db "bin2asm <input.bin> <output.asm>",13,10,0

txt_input:	db "Input file: <",0
txt_output:	db "Output file: <", 0
crlf:		db ">",13, 10, 0

_main:
	ld hl, banner
	call printZ

	ld a, (argc)
	cp 2
	jp c, show_usage

	ld hl, txt_input
	call printZ
	ld hl, (input_name)
	call printZ
	ld hl, crlf
	call printZ

	ld hl, txt_output
	call printZ
	ld hl, (output_name)
	call printZ
	ld hl, crlf
	call printZ

	ld hl, (input_name)
	ld c, fa_read
	MOSCALL mos_fopen
	or a
	jp z, i_open_err
	ld (ihandle), a

	ld hl, (output_name)
	ld c, fa_write+fa_cr_always
	MOSCALL mos_fopen
	or a
	jp z, o_open_err
	ld (ohandle), a

store_loop:
	ld a, (ihandle)
	ld c, a
	MOSCALL mos_feof
	or a
	jr nz, exit

	ld a, '\t'
	call write
	ld a, 'd'
	call write
	ld a, 'b'
	call write
	ld a, ' '
	call write
	ld a, '$'
	call write

	ld a, (ihandle)
	ld c, a
	MOSCALL mos_fgetc
	call write_hex
	
	ld a, 13
	call write
	ld a, 10
	call write
	jp store_loop


exit:	
; Close all files
	ld c, 0
	MOSCALL mos_fclose
	ret

; A - byte to write
write:
	ld b, a
	ld a, (ohandle)
	ld c, a
	MOSCALL mos_fputc
	ret

write_hex:
	ld h, a
	call @num1
	ld a, h
	call @num2
	ret
@num1:
	rra
	rra
	rra
	rra
@num2:
	or $f0
	daa
	add a,$a0
	adc a,$40
	push hl
	call write
	pop hl
	ret

o_open_err:
	ld a, (ihandle)
	ld c, a
	MOSCALL mos_fclose

	ld hl, @msg1
	jp printZ
@msg1:	db "Output file can't be created", 13, 10, 0

i_open_err:
	ld hl, @msg
	jp printZ
@msg:	db "Source file doesn't exists", 13,10, 0

show_usage:
	ld hl, usage
	call printZ
	ret

ihandle:	db 0
ohandle:	db 0
