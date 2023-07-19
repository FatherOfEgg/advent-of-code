include '../../include/z80.inc'
include '../../include/ti83plusg.inc'
include '../../include/tiformat.inc'
format ti executable 'TOVAR'

inputLen 			equ (inputend - input)

    ld hl, progName
    rst rMOV9TOOP1      ; OP1 = the name of this program, which is "TOVAR"
    bcall _Arc_Unarc	; Archive this program to make room in ram to be safe
    
	ld hl, varName
    rst rMOV9TOOP1      ; OP1 = AppVar name, which is "INPUT"

    ld hl, inputLen 	; Size of AppVar
    bcall _CreateAppVar

    ; First 2 bytes are length
    inc de
    inc de

	ld hl, input
	ld bc, inputLen
	ldir

	ld hl, varName
    rst rMOV9TOOP1      ; OP1 = AppVar name
	bcall _Arc_Unarc

	ld hl, progName
    rst rMOV9TOOP1      ; OP1 = program name
    bcall _Arc_Unarc	; Unarchive this program

    ret

progName:
    db ProgObj, "TOVAR", 0

varName:
	db AppVarObj, "INPUT", 0

input:
file 'input'
inputend: