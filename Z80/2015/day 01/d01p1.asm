include '../../include/z80.inc'
include '../../include/ti83plusg.inc'
include '../../include/tiformat.inc'
format ti executable 'D01P1'

	; ( 40 00101000b UP
	; ) 41 00101001b DOWN

length 			equ (inputend - input)

	bcall _ClrLCDFull

	call flooring

	ld hl, length	; Theoretical max floor we could be on, which is length
	or a			; Clear carry
	sbc hl, de		; Subtract result from flooring, which will give us how many floors we will go up
	or a			; Clear carry
	sbc hl, de		; Subtract again to get what floor we're currently on

	xor a
	ld (CurRow), a
	ld (CurCol), a

	bcall _DispHL
	bcall _NewLine
	ret

flooring:
	ld hl, input
	ld bc, length	; Loop counter
	ld de, 0		; Output is how many floors we will go down. Start at 0, and increment if ')'
.loop:
	rrc (hl)
	inc hl
	jp nc, .noIncrement
	inc de
.noIncrement:
	dec bc
	ld a, b
	or c
	jp nz, .loop
	ret

input:
file 'input'
inputend: