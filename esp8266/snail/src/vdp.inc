;; Inits screen
vdp_init:
	ld hl, @cmd
	ld bc, vdp_cls-@cmd
	rst.lil $18
	ret
@cmd:
	db 22, 0 ;; Mode 0(640x480)
	db 23, 1, 0 ;; Hide cursor
	db 23, 27, 0, 0
	db 23, 27, 1
	dw 32, 32
	incbin "info.rgba"

@end:	db 0

;; Clear screen
vdp_cls:
	ld a, 12
	jp putc


cursor_on:
	ld hl, @cmd
	ld bc, @end-@cmd
	rst.lil $18
	ret
@cmd:
	db 23, 1, 1
@end:

cursor_off:	
	ld hl, @cmd
	ld bc, @end-@cmd
	rst.lil $18
	ret
@cmd:
	db 23, 1, 0
@end:

;; DE - xy
goto_xy:
	push de
	ld a, 31
	call putc
	pop de
	push de
	ld a, e
	call putc
	pop de
	ld a, d
	call putc
	ret

; A - color
set_fg:
	and 63
set_color:	
	push af
	ld a, 17
	call putc
	pop af
	call putc
	ret

; A - color
set_bg:
	or $80
	jr set_color

vdp_close:
	ld hl, @cmd
	ld bc, @end-@cmd
	rst.lil $18
	ret
@cmd:
	db 22, 1
	db 17, 7, 17, $80
	db 23, 1, 1
	db 12
@end:

;; Print not more than 70 symbols and waiting for any separator
;; like cr/lf/tab/zero
;; HL - string ptr
print70G:
	ld b, 70
@loop:
	ld a, (hl)
	
	and a
	ret z

	cp 13
	ret z

	cp 10
	ret z
	
	cp 09
	ret z

	cp 32
	jr c, @skip

	push bc
	push hl
	call putc
	pop hl
	pop bc
	dec b
@skip:
	inc hl
	;; This happens cause we need skip unprintable things
	ld a, b
	or a
	ret z
	jr @loop

;; Print not more than 70 symbols and waiting for text line separator
;; HL - string ptr
print80T:
	ld b, 79
@loop:
	ld a, (hl)
	
	and a
	ret z

	cp 13
	ret z

	cp 10
	ret z
	
	cp 32
	jr c, @skip

	push bc
	push hl
	call putc
	pop hl
	pop bc
	dec b
@skip:
	inc hl
	;; This happens cause we need skip unprintable things
	ld a, b
	or a
	ret z
	jr @loop


ui_header:
	xor a
	call set_bg

	call vdp_cls
	ld a, 4
	call set_bg
	ld a, 11
	call set_fg

	ld hl, @title
	call printZ	

	xor a 
	call set_bg
	ld a, 11
	call set_fg

	ld hl, @line
	call printZ
	ld de, $0108
	call goto_xy

	ld hl, host
	call printZ
 
	call ui_buttons
	ret
@title:
	db " Snail - Gopher browser for Agon Light v.0.2 (c)"
	db " 2023 Aleksandr Sharikhin       "
	db 0
@line:
	db " Host: ["
	ds 64, 32
	db "]", 0

buttons_total: equ 5
ui_buttons:
	ld de, $0200
	call goto_xy
	call real_video
	ld hl, spacer
	call printZ

	ld b, buttons_total
	ld a, (selected_button)
	ld c, a
	ld a, buttons_total+1
	sub c
	ld c, a

	ld hl, @buttons
@loop:
	push bc

	push hl
	call real_video

	ld a, 32
	call putc
	pop hl
	pop bc
	push bc
	push hl

	ld a, b
	cp c
	push af
	call z, inv_video
	pop af
	call nz, real_video

	pop hl
	call printHL
	pop bc
	djnz @loop

	ld a, 13
	call putc
	ld a, 10
	call putc

	ret
@buttons:
	db "[Back]", 0
	db "[Home]", 0
	db "[Reload]", 0
	db "[Open server]",0
	db "[Quit]", 0

printHL:
	ld a, (hl)
	inc hl
	and a
	ret z
	push hl
	call putc
	pop hl
	jr printHL


inv_video:
	ld a,4 
	call set_bg
	ld a, 15
	call set_fg
	ret

real_video:
	ld a, 7
	call set_bg
	ld a, 4
	call set_fg
	ret

selected_button: db 0

spacer: ds 80, 32
	db 13, 11, 0


msg_box:
	ld hl, @cmd
	ld bc, @end-@cmd
	rst.lil $18
	ret
@cmd:
	db 31, 8, 25
	db 17, $84, 17, 15
	ds 64, 32

	db 31, 8, 26
	ds 64, 32

	db 31, 8, 27
	ds 64, 32

	db 31, 8, 28
	ds 64, 32

	db 31, 8, 29
	ds 64, 32

	db 31, 8, 30
	ds 64, 32

	db 23, 27, 0, 0
	db 23, 27, 3
	dw 72, 208

	db 31, 15, 27
@end:


