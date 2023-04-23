HISTORY_DEPTH: EQU 4

history_push:
	ld de, history_end - 1
	ld hl, history_preend - 1
	ld bc, 4167*HISTORY_DEPTH
	lddr
	ret

history_pull:
	ld de, row_type
	ld hl, history_buffer
	ld bc, HISTORY_DEPTH-1*4167
	ldir
	ret

row_type:
	db 0 ;; 0 for gopher, 1 for plain text
host:
	ds 64, 0
str_port:
	ds 6, 0
request:
	ds 4096

history_buffer:
	ds HISTORY_DEPTH-1*4167
history_preend:
	ds 4167
history_end:


