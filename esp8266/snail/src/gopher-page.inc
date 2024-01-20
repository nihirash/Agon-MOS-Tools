UNKN: EQU 7
INFO: EQU 15
LINK: EQU 10
TEXT: EQU 02
DOWN: EQU 09
INPT: EQU 14

page_offset:
	dl 0

cursor_position:
	db 0
on_screen_position:
	db 0
last_page:
	db 0
; A - gopher line first char
get_type:
	cp 'i'
	jr z, @r_info

	cp '7'
	jr z, @r_input

	cp '1'
	jr z, @r_link

	cp '0'
	jr z, @r_text

	cp '9'
	jr z, @r_down

	cp 'I'
	jr z, @r_down

	cp '5'
	jr z, @r_down

	ld a, UNKN
	ret
@r_down:
	ld a, DOWN
	ret
@r_info:
	ld a, INFO
	ret
@r_input:
	ld a, INPT
	ret
@r_link:
	ld a, LINK
	ret
@r_text:
	ld a, TEXT
	ret

text_render:
	call ui_header

	xor a 
	ld (last_page), a

	call set_bg
	ld a, 15
	call set_fg

	ld de, $0400
	call goto_xy
	ld b, 55 ;;PER_PAGE
@loop:
	push bc
	ld hl, (page_offset)

	ld a, 55
	sub b
	
	ld bc, 0
	ld c, a
	add hl, bc

	push hl
	pop bc
	call find_line

	ld a, l
	or h
	jr z, @oops

	call print80T
@oops2:
	ld a, 13
	call putc
	ld a, 10
	call putc

	pop bc
	djnz @loop
	ret
@oops:
	ld a, 1
	ld (last_page), a

	jr @oops2

text_loop:
	MOSCALL mos_getkey

	cp 27
	jp z, go_menu

	push af
	cp 10
	call z, text_down
	pop af
	
	cp 11
	call z, text_up
	jr text_loop

text_up:
	ld hl, (page_offset)
	ld a, l
	or h
	ret z

	ld de, 55
	or a
	sbc hl, de
	ld (page_offset), hl
	jp text_render
	
text_down:
	ld a, (last_page)
	and a
	jp nz, text_render

	ld hl, (page_offset)
	ld de, 55
	add hl, de
	ld (page_offset), hl
	jp text_render

gopher_page_init:
	ld hl, 0
	ld (page_offset), hl

	xor a
	ld (cursor_position), a
	ld (on_screen_position), a
	ret

cursor_down:
	call hide_cursor
	ld a, (cursor_position)
	inc a
	cp 55
	jr nc, page_down
	ld (cursor_position), a
	jp show_cursor

scroll_down:
	call hide_cursor
page_down:
	xor a
	ld (cursor_position), a

	ld a, (last_page)
	or a
	jp nz, gopher_render

	ld hl, (page_offset)
	
	ld de, 55
	add hl,de
	ld (page_offset), hl

	jp gopher_render

cursor_up:
	call hide_cursor
	ld a, (cursor_position)
	or a
	jr z, page_up
	dec a
	ld (cursor_position), a
	jp show_cursor

scroll_up:
	call hide_cursor
page_up:
	ld hl, (page_offset)

	ld a, h
	or l
	jp z, show_cursor

	ld de, 55
	or a
	sbc hl, de

	ld (page_offset), hl
	ld a, 54
	ld (cursor_position), a
	jp gopher_render	

gopher_loop:
	MOSCALL mos_getkey

	cp 27
	jp z, go_menu

	cp 13
	jp z, navigate

	push af
	cp 10
	call z, cursor_down
	pop af
	
	push af
	cp 11
	call z, cursor_up
	pop af

	push af
	cp 8
	call z, scroll_up
	pop af
	
	push af
	cp 21
	call z, scroll_down
	pop af

	jr gopher_loop

show_cursor:
	call active_row
	jp cursor_render
hide_cursor:
	call usual_row
cursor_render:
	ld a, (cursor_position)
	add a, 4
	ld d, a
	ld e, 0
	call goto_xy

	ld hl, spacer
	call printZ

	ld hl, (page_offset)
	ld bc, 0
	ld a, (cursor_position)
	ld c, a
	add hl, bc
	push hl
	pop bc
	call find_line
	ld a, l
	or h
	ret z
	ld a, (hl)
	inc hl
	push hl
	call get_type
	call set_fg

	ld hl, row_offset
	call printZ
	pop hl
	jp print70G


	
navigate:
	ld hl, (page_offset)
	ld bc, 0
	ld a, (cursor_position)
	ld c, a
	add hl, bc
	push hl
	pop bc
	call find_line

	ld a, (hl)
	call get_type

	cp LINK
	jp z, @do

	cp TEXT
	jp z, @do

	cp INPT
	jp z, @query

	cp DOWN
	jp z, @down

	jp gopher_loop
@down:
	call gopher_extract
	call gopher_download
	jp   m_back
@do:
	call gopher_extract
	jp   gopher_process
@query:
	call gopher_extract

	call msg_box
	ld hl, q_msg
	call printZ

	ld hl, request
	ld bc, 4096
	ld a, 13
	cpir
	
	dec hl

	ld a, 9
	ld (hl), a
	inc hl
	push hl
	call cursor_on
	pop hl
	ld bc, 60
	ld e, 1
	MOSCALL mos_editline
	cp 27
	push af
	call cursor_off
	call gopher_render
	pop af
	jp z, gopher_loop
	
	ld hl, request
	ld bc, 4096
	xor a
	cpir

	dec hl

	ld a, 13
	ld (hl), a
	inc hl

	ld a, 10
	ld (hl), a
	inc hl

	xor a
	ld (hl), a
	jp gopher_process
	
q_msg:
	db "Enter your query:", 31, 15, 28, 0

find_first_link:
	ld b, 55
@loop:
	push bc
	ld hl, (page_offset)
	ld a, 55
	sub b
	ld (@current_line), a

	ld bc, 0
	ld c, a
	add hl, bc
	push hl
	pop bc
	call find_line
	
	ld a, l
	or h
	jr z, @oops

	ld a, (hl)
	call get_type
	cp UNKN
	jr z, @skip
	
	cp INFO
	jr z, @skip

	ld a, (@current_line)
	ld (cursor_position), a
	jr @oops
@skip:
	pop bc
	djnz @loop
	ret
@oops:
	pop bc
	ret
@current_line:
	db 0

gopher_render:
	call ui_header

	xor a 
	ld (last_page), a
	call find_first_link

	ld de, $0400
	call goto_xy
	ld b, 55 ;;PER_PAGE
@loop:
	push bc
	ld hl, (page_offset)
	
	ld a, 55
	sub b
	ld (on_screen_position),a
	
	ld bc, 0
	ld c, a
	add hl, bc

	push hl
	pop bc
	call find_line

	ld a, l
	or h
	jr z, @oops

	call render_row
@oops2:
	ld a, 13
	call putc
	ld a, 10
	call putc

	pop bc
	djnz @loop
	ret
@oops:
	ld a, 1
	ld (last_page), a

	jr @oops2



;; HL - pointer to line
render_row:
	push hl

	ld a, (cursor_position)
	ld b, a
	ld a, (on_screen_position)
	cp b

	push af
	call z, active_row
	pop af
	call nz, usual_row

	ld hl, spacer
	call printZ ;; Clean up line
	ld hl, row_offset
	call printZ

	pop hl
	ld a, (hl)
	inc hl
	push hl
	call get_type
	call set_fg
	pop hl
	call print70G
	ret
row_offset:
	ds 5, 32
	db 0

active_row:
	ld a, 4
	call set_bg
	ret

usual_row:
	xor a
	call set_bg
	ret

; BC - line number(cheching only 16 bits)
find_line:
	ld hl, page_buffer
	ld a, b
	or c
	jr z, @check_empty
@loop:
	ld a, (hl)
	and a 
	jr z, @empty
	cp 13
	inc hl
	jr z, @check_lf
	cp 10
	jr z, @check_next
	jr @loop
@check_next:
	and a
	jr z, @empty
	dec bc
	ld a, b
	or c
	jr nz, @loop
	ret
@check_lf:
	ld a, (hl)
	cp 10
	jr nz, @check_next
	inc hl
	jr @check_next
@check_empty:
	ld a, (hl)
	and a
	ret nz
@empty:
	ld hl, 0
	ret

	
