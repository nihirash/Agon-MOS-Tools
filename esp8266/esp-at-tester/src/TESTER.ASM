MAX_ARGS:	equ 1
	include "crt.inc"
	include "mos.inc"
	include "wifi.inc"

banner:
	db "ESP8266 AT-commands tester",13,10
	db "(c) 2023 Nihirash",13,10,0

_main:
	ld hl, banner
	call printZ

	call esp_init
	call esp_repl
exit:
	call esp_free
	ret

esp_repl:
	ld a, 13
	call putc
	ld a, 10
	call putc

	ld hl, buffer
	ld bc, 80
	ld e, 1
	MOSCALL mos_editline

	ld a, 13
	call putc
	ld a, 10
	call putc

	ld hl, buffer
	ld a, (hl)
	cp 'B'
	jr nz, @eval
	
	inc hl
	ld a, (hl)
	cp 'Y'
	jr nz, @eval
	
	inc hl
	ld a, (hl)
	cp 'E'
	jr nz, @eval
	jr exit

@eval:
	ld hl, buffer
	call ok_err_cmd
	jr esp_repl

buffer: ds 80


