	ASSUME ADL=1
;; USE org $40000 for usual application
;; or $B0000 for MOS command
	org $40000
	jp _start
	align 64
	db "MOS" ; Header
	db 00    ; Version
	db 01 	 ; ADL
_start:
	push ix
	push iy

	ld ix,argv
	call _parse_args
	ld a,c
	ld (argc),a

	call _main
	pop iy
	pop ix

	ld hl,0
	ret

_parse_args:
	call _skip_spaces
	ld bc,0
	ld b,MAX_ARGS
_parse1:
	push bc
	push hl
	call _get_token
	ld a,c
	pop de
	pop bc
	and a
	ret z

	ld (ix+0),de
	push hl
	pop de
	call _skip_spaces
	xor a
	ld (de),a
	inc ix
	inc ix
	inc ix
	inc c
	ld a, c
	cp b
	jr c,_parse1
	ret

_get_token:
	ld c,0
@loop:
	ld a,(hl)
	or a
	ret z

	cp 13
	ret z

	cp 32
	ret z

	inc hl
	inc c
	jr @loop

_skip_spaces:
	ld a,(hl)
	cp 32
	ret nz
	inc hl
	jr _skip_spaces

argc:	db	0
argv:	ds	3*MAX_ARGS

