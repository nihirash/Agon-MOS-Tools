MAX_ARGS: EQU 2
	include "crt.inc"
	include "tcp.inc"
	include "vdp.inc"
	include "gopher-page.inc"
	include "gopher.inc"
	include "top-menu.inc"
_main:
	call vdp_init

	call msg_box
	ld hl, txt_esp
	call printZ

	call tcp_init

	call go_home

	call esp_free
	call vdp_close
	ret
go_home:
	xor a
	ld (selected_button), a 
	ld hl, home_page
	ld de, page_buffer
	ld bc, home_page_end - home_page
	ldir

	call gopher_page_init
	call gopher_render
	jp gopher_loop
	

txt_esp:
	db "Initing ESP8266. If we stuck here - issue with it!", 0

home_page:
	incbin "home.gph"
	dl 0
home_page_end:
	
	include "history.inc"
page_buffer:
	ds 3,0
