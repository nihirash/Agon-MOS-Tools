go_menu:
	ld a, 1
	ld (selected_button), a
menu_loop:
	call ui_buttons
	MOSCALL mos_getkey

	cp 27
	jp z, exit

	cp 8
	jr z, menu_left

	cp 21
	jr z, menu_right

	cp 13
	jr z, menu_item

	jr menu_loop

menu_left:
	ld a, (selected_button)

	cp 1
	jr z, menu_loop

	dec a
	ld (selected_button), a
	jr menu_loop

menu_right:
	ld a, (selected_button)
	
	cp 5
	jr nc, menu_loop

	inc a
	ld (selected_button), a
	jr menu_loop

menu_item:
	ld a, (selected_button)
	cp 5
	ret z

	cp 3
	jp z, gopher_process

	cp 4
	jr z, domain

	cp 1
	jr z, m_back

	cp 2
	jp z, go_home

	jr menu_loop

m_back:
	call history_pull
	jp gopher_process

domain:
	ld de, $0108
	call goto_xy

	xor a
	call set_bg
	ld a, 11
	call set_fg

	call cursor_on
	
	ld hl, host
	ld bc, 64
	ld e, 0
	MOSCALL mos_editline
	push af
	call cursor_off
	pop af
	cp 27
	jp z, text_loop
	
	ld hl, request
	ld a, '/'
	ld (hl), a
	inc hl

	ld a, 13
	ld (hl), a
	inc hl

	ld a, 10
	ld (hl), a
	inc hl

	xor a
	ld (hl), a

	ld hl, str_port
	ld a, '7'
	ld (hl), a
	inc hl

	ld a, '0'
	ld (hl), a
	inc hl

	xor a
	ld (hl), a
	ld (row_type), a
	jp gopher_process 

exit:
	xor a
	ld (selected_button), a
	ld a, (row_type)
	and a 
	jr z, @go
	call text_render
	jp text_loop
@go:
	call gopher_render
	jp gopher_loop

