MAX_ARGS:	equ 1
	include "crt.inc"
	include "mos.inc"
	include "wifi.inc"

banner:
	db "ESP8266 Update utility ", 13,10
	db "(c) 2023 Nihirash", 13, 10, 0

_main:
	ld hl, banner
	call printZ

	call esp_init
	ld hl, at_update
	call ok_err_cmd
	jr z, err
	
	call esp_init
	
	ld hl, at_restore
	call ok_err_cmd
	

	ld hl, reconnect
	call printZ
exit:
	call esp_free
	ret
err:
	ld hl, err_msg
	call printZ
	jr exit
err_msg:
	db "Update error! Make sure that you'd connected to Access Point", 13, 10, 0

at_update:
	db "AT+CIUPDATE", 0

at_restore:
	db "AT+RESTORE", 0

reconnect:
	db 13, 10, "Run netman - reconfiguring ESP required", 13,10,0

