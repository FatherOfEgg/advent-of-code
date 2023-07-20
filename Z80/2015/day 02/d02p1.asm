include '../../include/z80.inc'
include '../../include/ti83plusg.inc'
include '../../include/tiformat.inc'
format ti executable 'D02P1'

	bcall _ClrLCDFull

	call openInput

	call calcTotalSquareFt
	xor a
	ld (CurRow), a
	ld (CurCol), a

	call printTotal

	call closeInput

    ret

printTotal:
	ld hl, totalSquareFt
	ld b, 4
	ld de, numberStringEnd - 1
	ld c, $0F
.loop:
	ld a, c
	and (hl)

	call itoa
	ld (de), a
	dec de

	ld a, (hl)
	rrca
	rrca
	rrca
	rrca
	and c

	call itoa
	ld (de), a
	dec de
	
	inc hl
	djnz .loop

	ex de, hl
	inc hl
	bcall _PutS
	bcall _NewLine
	ret

itoa:
	add '0'
	cp '0' + 10
	jr c, .isDigit
	add 'A' - '0' - 10
.isDigit:
	ret

calcTotalSquareFt:
	; DE = Pointer to input
	; BC = Input length
	ex de, hl
	ld c, (hl)
	inc hl
	ld b, (hl)
	inc hl
	ex de, hl
.loop:
	call atoi
	ld (length), hl
	call atoi
	ld (width), hl
	call atoi
	push bc, de
	push hl		; Height

	ld hl, (length)
	ld de, (width)

	push de

	; L x W
	call multHLxDE
	ld (smallestArea), de
	call add2xAreaToTotal

	pop hl, de
	push de

	; W x H
	call multHLxDE
	call updateSmallestArea
	call add2xAreaToTotal

	ld hl, (length)
	pop de

	; L x H
	call multHLxDE
	call updateSmallestArea
	call add2xAreaToTotal

	; Add smallest area
	ld de, (smallestArea)
	call addAreaToTotal

	pop de, bc
	ld a, b
	or c
	jp nz, .loop
	ret

; Ascii To Int
; Converts until 'x' or '\n'
; Input: DE = address of ascii to be converted
; Output: HL
; Destroys: A
atoi:
	push bc
	ld hl, 0
.loop:
	ld a, (de)
	inc de

	pop bc
	dec bc
	
	cp 'x'
	ret z
	cp $0A	; New line
	ret z

	push bc
	; Multiply HL by 10
	add hl, hl	; HL * 2
	ld b, h		; BC = HL * 2
	ld c, l
	add hl, hl	; Hl * 4
	add hl, hl	; Hl * 8
	add hl, bc	; HL * 10

	sub '0'
	ld b, 0
	ld c, a

	add hl, bc
	jp .loop

; Multiplies HL by DE
; Input: HL = first dimension, DE = second dimension
; Output: DE
; Destroys: A, BC, HL
multHLxDE:
	; Put bigger number into HL to be multiplicand
	xor a
	sbc hl, de
	add hl, de

	jp nc, .skipSwap
	ex de, hl
.skipSwap:
	; BC is multiplier, and DE is product
	ld b, d
	ld c, e
	ld d, a
	ld e, a
.loop:
	srl b
	rr c
	jp c, .add
	
	ld a, b
	or c
	ret z

	jp .skipAdd
.add:
	ex de, hl
	add hl, de
	ex de, hl
.skipAdd:
	add hl, hl
	jp .loop

; Input: DE = value of new area
; Output: None
; Destroys: A, HL
updateSmallestArea:
	ld hl, (smallestArea)
	or a
	sbc hl, de
	ret c		; Return if new area is not smaller
	ld (smallestArea), de	; Store new small area
	ret

; Input: DE = Area to be added to total
; Output: None
; Destroys: A, HL
add2xAreaToTotal:
	; Multiply current area by 2
	ex de, hl
	add hl, hl
	ex de, hl
addAreaToTotal:
	ld hl, (totalSquareFt)
	add hl, de
	ld (totalSquareFt), hl
	ret nc

	ld hl, (totalSquareFt + 2)
	inc hl
	ld (totalSquareFt + 2), hl
	ret

length:
	dw 0
width:
	dw 0
smallestArea:
	dw 0

totalSquareFt:
	dd 0

numberString:
	db 8 dup ' '
numberStringEnd:
	db 0

; Input: None
; Output: DE = pointer to start of data
; Destroys: All
openInput:
	ld hl, progName
    rst rMOV9TOOP1    	; OP1 = program name, which is "D02P1"
	bcall _Arc_Unarc	; Archive program to make room

	ld hl, varName
    rst rMOV9TOOP1    	; OP1 = AppVar name, which is "INPUT"
	bcall _Arc_Unarc	; Unarchive AppVar
	ld hl, varName
    rst rMOV9TOOP1    	; OP1 = AppVar name
    bcall _ChkFindSym	; Look up

	ret

; Input: None
; Output: None
; Destroys: All
closeInput:
	ld hl, varName
    rst rMOV9TOOP1    	; OP1 = AppVar name
	bcall _Arc_Unarc	; Archive AppVar

	ld hl, progName
    rst rMOV9TOOP1    	; OP1 = program name
	bcall _Arc_Unarc	; Unarchive this program

	ret

progName:
	db ProgObj, "D02P1", 0
varName:
	db AppVarObj, "INPUT", 0