include '../../include/z80.inc'
include '../../include/ti83plusg.inc'
include '../../include/tiformat.inc'
format ti executable 'D01P2'

	; ( 40 00101000b UP
	; ) 41 00101001b DOWN

length 			equ (inputend - input)

	bcall _ClrLCDFull

	call findFirstBasementOffset

	xor a			; Make a 0, as well as clear carry
	ld (CurRow), a
	ld (CurCol), a

	ld de, input
	sbc hl, de

	bcall _DispHL
	bcall _NewLine
	ret

findFirstBasementOffset:
	ld hl, input
	ld bc, length	; Loop counter
	ld de, 0		; Current floor
.loop:
	rrc (hl)
	inc hl
	jp nc, .add
	dec de
	jp .checkIfFirstBasement
.add:
	inc de
.checkIfFirstBasement:
	push hl
	ld hl, -1
	xor a
	sbc hl, de
	pop hl
	ret z

	dec bc
	ld a, b
	or c
	jp nz, .loop
	ret

input:
file 'input'
inputend: