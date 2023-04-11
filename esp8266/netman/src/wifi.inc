
esp_init:
	call uart_init

	ld hl, @cmd_at
	call ok_err_cmd
	jp z, @esp_init_error

	ld hl, @cmd_e0
	call ok_err_cmd
	jp z, @esp_init_error

	ld hl, @cmd_mode
	call ok_err_cmd
	jp z, @esp_init_error

	ld hl, @cmd_auto
	call ok_err_cmd

	ld hl, @cmd_info
	call ok_err_cmd

	ld hl, @cmd_mux
	call ok_err_cmd

	ret
@esp_init_error:
	xor a
	ret
@cmd_at:	db "AT",0
@cmd_e0:	db "ATE0",0
@cmd_mode:	db "AT+CWMODE_DEF=1",0
@cmd_auto:	db "AT+CWAUTOCONN=1",0
@cmd_info:	db "AT+CIPDINFO=0",0
@cmd_mux:	db "AT+CIPMUX=0",0

esp_free:
	MOSCALL mos_uclose
	ret

uart_init:
	ld ix, serial_config
	MOSCALL mos_uopen
	ret

	macro UART_WAIT byte, if_err
	call uart_read
	cp byte
	jp nz, if_err
	endmacro


; hl -> cmd
; a <- 1 ok(nz flag),0 err(z flag)
ok_err_cmd:
	call uart_writeZ
	ld a, 13
	call uart_write
	ld a, 10
	call uart_write
ok_err_loop:
	call uart_read

	cp 'O'
	jr z, r_ok

	cp 'E'
	jp z, r_err

	cp 'F'
	jp z, r_fail

	jr ok_err_loop

r_ok:
	UART_WAIT 'K', ok_err_loop
	UART_WAIT 13, ok_err_loop

	ld a, 1
	or a
	ret

r_err:
	UART_WAIT 'R', ok_err_loop
	UART_WAIT 'R', ok_err_loop
	UART_WAIT 'O', ok_err_loop
	UART_WAIT 'R', ok_err_loop
	UART_WAIT 13, ok_err_loop

	xor a
	ret
r_fail:
	UART_WAIT 'A', ok_err_loop
	UART_WAIT 'I', ok_err_loop
	UART_WAIT 'L', ok_err_loop
	UART_WAIT 13, ok_err_loop

	xor a
	ret

; A - byte that was read
uart_read:
	MOSCALL mos_ugetc
	ret

; HL - pointer to string
uart_writeZ:
	ld a, (hl)
	and a
	ret z
	push hl
	call uart_write
	pop hl
	inc hl
	jr uart_writeZ

; A - byte to send
uart_write:
	ld c, a
	MOSCALL mos_uputc
	ret


serial_config:
	dl 115200
	db 8 ; data bits
	db 1 ; 1 stop bit
	db 0 ; parity
	db 0 ; flow control
	db 0 ; interrupts


