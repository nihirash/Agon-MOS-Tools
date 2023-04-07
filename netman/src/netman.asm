;
; Network manager for Agon Light and ESP8266 on UART1
; (c) 2023 Aleksandr Sharikhin aka Nihirash
;
; Licensed under MIT


MAX_ARGS:	equ	2
	include "crt.inc"
	include "mos.inc"
	include "wifi.inc"
banner:
	db "Network Manager for ESP8266 v. 0.1.0",13,10
	db "(c) 2023 Aleksandr Sharikhin", 13, 10, 13, 10
	db "Initing UART and WiFi chip",13,10,0
_main:
	ld hl, banner
	call printZ

	call esp_init
	jp z, err

	ld hl, cmd_quit
	call ok_err_cmd

	ld hl, cmd_list
	call uart_writeZ
	
	ld hl, txt_ap_list
	call printZ

;; Requesting AP list(AT+CWLAP command)
load_list:
	call uart_read

	cp $2b ; + - begin of data line
	jr z, @r_plus

	cp 'O'
	jr z, @r_ok

	cp 'E'
	jr z, @r_err
	jr load_list
@r_plus:
	call uart_read
	cp 'C'
	jr nz, load_list

	call uart_read
	cp 'W'
	jr nz, load_list

	call uart_read
	cp 'L'
	jr nz, load_list
	;; By +CWL we can understand that it's our required line
	jr load_ap
@r_ok:
	call uart_read
	cp 'K'
	jr nz, load_list
	
	call uart_read
	cp 13
	jr nz, load_list
	;; It's "OK\r\n" response - end of list
	jp loaded_list
@r_err:
	call uart_read
	cp 'R'
	jr nz, load_list
	
	call uart_read
	cp 'R'
	jr nz, load_list
	;; "ERR" - looks too similar to ERROR
	jp err
;; Line format a bit more complex that we using. It looks like:
;; +CWLAP: (4, "name", -83, "c0:12:23:45:67....",2-7,0)
;; We looking only for "name" part. Starting search by first quot.mark
load_ap:
	call uart_read
	cp '"'
	jr nz, load_ap
;; And show name until we meet quot. mark again
load_name:
	call uart_read
	cp '"'
	jr z, loaded_name
	call putc
	jr load_name
;; Bang new line and space - it looks nice :)
loaded_name:
	ld a, 13
	call putc
	ld a, 10
	call putc
	ld a, 32
	call putc
	jp load_list
;; Here we'll ask about selecting access point and entering password
loaded_list:
	ld hl, txt_ssid
	call printZ
	ld hl, ssid
	ld bc, 80
	ld e, 1
	MOSCALL mos_editline
	cp 27
	jp z, abort

	ld hl, txt_pass
	call printZ
	ld hl, pass
	ld bc, 80
	ld e, 1
	MOSCALL mos_editline
	cp 27
	jp z, abort


;; AT+CWJAP="ssid","password"\r\n
	ld hl, cmd_cwjap
	call uart_writeZ

	ld hl, ssid
	call uart_writeZ

	ld hl, cmd_cwjap2
	call uart_writeZ

	ld hl, pass
	call uart_writeZ

	ld hl, cmd_cwjap3
	call uart_writeZ
	
	ld a, 13
	call putc
connect:
	;; We using ECHOed uart read to show connection stages on screen :)
	;; Like "WIFI GOT IP" etc
	call echo_read
	
	cp 'O'
	jr z, @r_ok

	cp 'E'
	jr z, @r_err

	cp 'F'
	jr z, @r_fail

	jr connect
@r_ok:
	call echo_read
	cp 'K'
	jr nz, connect

	call echo_read
	cp 13
	jr z, exit
	jr connect
@r_err:
	call echo_read
	cp 'R'
	jr nz, connect
	
	call echo_read
	cp 'R'
	jr nz, connect
	jr err
@r_fail:
	call echo_read
	cp 'A'
	jr nz, connect
	
	call echo_read
	cp 'I'
	jr nz, connect
	jr err

abort:
	ld hl, @msg
	call printZ
	jr exit
@msg:	db 13,10,"Aborted", 13, 0	

;; When all done - free UART and be nice :-)
exit:
	ld a, 10
	call putc
	call esp_free

	ret
err:
	ld hl, @err
	call printZ
	call esp_free
	ret
@err: db 13,10, "Error happens on ESP's init stage",13,10,0

;; Just additional uart_read procedure for last step
echo_read:
	call uart_read
	push af
	call putc
	pop af
	ret


txt_ssid:	db 13,10,"Enter SSID:",13,10,0
txt_pass:	db 13,10,"Enter password:",13,10,0
txt_ap_list:	db "SSIDs: ", 13, 10, 32, 0


cmd_quit:	db "AT+CWQAP", 0
cmd_list:	db "AT+CWLAP", 13, 10, 0
cmd_cwjap:	db "AT+CWJAP=\"",0
cmd_cwjap2:	db "\",\"",0
cmd_cwjap3:	db "\"",13,10,0


ssid: ds 80
pass: ds 80

