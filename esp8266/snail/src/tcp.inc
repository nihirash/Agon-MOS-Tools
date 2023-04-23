; Writes to UART number from HL
;
; Important - it doesn't support full range of numbers(only enough for
; sending requests via ESP8266)
; HL -> number
send_int:
	ld bc, -1000
	call @num1
	ld bc, -100
	call @num1
	ld c, -10
	call @num1
	ld c,-1
@num1:
	ld a,'0'-1
@num2:
	inc a
	add hl, bc
	jr c, @num2
	sbc hl, bc
	push hl
	call uart_write
	pop hl	
	ret

; Reads byte from uart and converts to integer
; hl - result
recv_int:
	ld hl, 0
	ld bc, 0
@loop:
	push hl
	call uart_read
	pop hl
	cp ':'
	ret z

	sub $30	
	ld c, l
	ld b, h
	add hl, hl
	add hl, hl
	add hl, bc
	add hl, hl
	ld c, a
	ld b, 0
	add hl, bc
	jr @loop


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
ok_err_crlf:
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


tcp_init:
	call esp_init

	ld hl, cip_close
	call ok_err_cmd

	ld hl, @pasv
	call ok_err_cmd
	ret
@pasv:	db "AT+CIPRECVMODE=1",0


tcp_close:
	ld hl, cip_close
	call ok_err_cmd
	ret

;; HL - pointer to host
;; DE - pointer to port
tcp_open:
	push de
	push hl
	ld hl, cip_close
	call ok_err_cmd
	
	ld hl, cip_start1
	call uart_writeZ

	pop hl
	call uart_writeZ
	
	ld hl, cip_start2
	call uart_writeZ

	pop hl
	call ok_err_cmd
	ret

;; HL <- buffer
tcp_writeZ:
	push hl
	call strlen
	push bc

	ld hl, cip_send	
	call uart_writeZ

	pop hl
	call send_int
	call ok_err_crlf
	jr z, @err
@wait:
	call uart_read
	cp 62 ;; > symbol
	jr nz, @wait

	pop hl
	call uart_writeZ
	call ok_err_loop	
	ret
@err:
	pop hl
	xor a
	ret

strlen:
	ld bc, 0
	push hl
@loop:
	ld a, (hl)
	and a
	jr z, @exit
	inc hl
	inc bc
	jr @loop
@exit:
	pop hl
	ret

;; HL <- buffer
;; BC -> bytes received
tcp_recv:
	push hl
	ld hl, cmd_recv
	call uart_writeZ
tcp_r_loop:
	call uart_read

	cp 43
	jp z,tcp_r_packet

	cp 'O'
	jr z,tcp_r_ok

	cp 'E'
	jr z,tcp_r_err
	jr tcp_r_loop
tcp_r_err:
	UART_WAIT 'R', tcp_r_loop
	UART_WAIT 'R', tcp_r_loop
	pop hl
	ld bc, 0
	xor a
	ret
tcp_r_ok:
	UART_WAIT 'K', tcp_r_loop
	UART_WAIT 13, tcp_r_loop
	pop hl
	or a
	ld bc, 0
	ret

tcp_r_packet:
	UART_WAIT 'C', tcp_r_loop
	UART_WAIT 'I', tcp_r_loop
	UART_WAIT 'P', tcp_r_loop
	UART_WAIT 'R', tcp_r_loop ;; +CIPRECVDATA
@comma:
	call uart_read
	cp 44 ;; comma
	jr nz, @comma
	call recv_int
	push hl
	pop bc

	pop hl
	push bc
@recv:
	push bc

	push hl
	call uart_read
	pop hl
	ld (hl), a
	pop bc

	inc hl
	dec bc

	ld a, b
	or c
	jr z, @end

	jr @recv
@end:
	ld a, 1
	or a
	pop bc
	ret	

cmd_recv:
	db "AT+CIPRECVDATA=1024",13,10,0


cip_send:
	db "AT+CIPSEND=", 0

cip_start1:
	db "AT+CIPSTART=\"TCP\",\"",0
cip_start2:
	db "\",",0

cip_close:
	db "AT+CIPCLOSE", 0
