
gopher_process:
	xor a
	ld (selected_button), a
	call gopher_load
	call gopher_page_init
	ld a, (row_type)
	and a 
	jr z, @go
	call text_render
	jp text_loop
	ret
@go:
	call gopher_render
	jp gopher_loop



gopher_extract:
	push hl
	call history_push
	pop hl
down_extract:
	ld a, (hl)
	cp '0'
	ld a, 1
	jr z, @write_type
	xor a
@write_type:
	ld (row_type), a

	ld a, 9 ; TAB
	ld bc, 1024
	cpir
	ld de, request
@req_ext:
	ld a, (hl)
	
	inc hl
	and a
	ret z
	cp 9
	jr z, @host_ext
	ld (de), a
	inc de
	jr @req_ext
@host_ext:
	ld a, 13
	ld (de), a
	inc de

	ld a, 10
	ld (de) ,a
	inc de

	xor a
	ld (de), a

	ld de, host
@host_loop:
	ld a, (hl)
	
	inc hl
	and a
	ret z
	cp 9
	jr z, @d_port_ext
	ld (de), a
	inc de
	jr @host_loop
@d_port_ext:
	xor a
	ld (de), a
	
	ld de, str_port
@d_port_loop:
	ld a, (hl)
	inc hl
	and a
	jr z, @done
	cp 9
	jr z, @done
	cp 13
	jr z, @done
	cp 10
	jr z, @done
	ld (de), a
	inc de
	jr @d_port_loop
@done:
	xor a
	ld (de), a
	ret

gopher_load:
	call msg_box
	ld hl, @msg
	call printZ

	ld hl, host
	ld de, str_port
	call tcp_open
	jp z, open_tcp_err

	ld hl, request
	call tcp_writeZ

	ld hl, page_buffer
@load:
	call tcp_recv
	jr z, @done
	jr @load	
@done:
	xor a 
	ld (hl), a
	inc hl
	ld (hl), a
	inc hl
	ld (hl), a
	inc a
	or a
	ret
@msg:	db "Document loading! Please wait!", 0

open_tcp_err:
	call msg_box
	ld hl, @msg_err
	call printZ
	xor a
	ret
@msg_err:
	db "Document fetch error!", 0

gopher_download:
	call cursor_on

	call find_filename

	call msg_box
	ld hl, str_out_file
	call printZ

	ld hl, down_filename
	ld bc, 60
	ld e, 0
	MOSCALL mos_editline
	call cursor_off

	ld c, 0
	MOSCALL mos_fclose

	ld hl, down_filename
	ld c, fa_write+fa_cr_always
	MOSCALL mos_fopen
	ld (ohandle), a

	ld hl, host
	ld de, str_port
	call tcp_open
	jp z, open_tcp_err

	ld hl, request
	call tcp_writeZ

	ld hl, page_buffer
download_loop:
	call tcp_recv
	jp z, download_exit
	jr download_loop

download_exit:
	ld de, page_buffer
	or a
	sbc hl, de

	ex de, hl
	ld a, (ohandle)
	ld c, a
	MOSCALL mos_fwrite

	ld c, 0
	MOSCALL mos_fclose
	ret	
ohandle:
	db 0

str_out_file:
	db "Output filename:", 31, 15, 28, 0

find_filename:
	ld de, request
	ld hl, request
@loop:
	ld a, (de)
	inc de
	cp '/'
	jr nz, @skip
	push de
	pop hl
@skip:
	and a
	jr z, @copy
	cp '?'
	jr z, @copy
	cp 13
	jr z, @copy
	jr @loop
@copy:
	ld de, down_filename
@cl:
	ld a, (hl)
	
	and a
	jr z, @done
	
	cp 13
	jr z, @done
	
	cp '?'
	jr z, @done
	
	ld (de), a

	inc hl
	inc de
	jr @cl

@done:
	xor a
	ld (de), a
	ret

down_filename:
	ds 64

