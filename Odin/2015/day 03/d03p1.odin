#+private file
package main

import "core:fmt"
import "core:os"
import "core:bufio"

INPUT_FILE :: "input.txt"

Position :: struct {
    x, y : int
}

main :: proc() {
    f, fErr := os.open(INPUT_FILE)

    if fErr != nil {
        assert(false, "Couldn't open: " + INPUT_FILE)
    }

    defer os.close(f)

    r: bufio.Reader
    bufio.reader_init(&r, os.stream_from_handle(f))
    defer bufio.reader_destroy(&r)

    numHouses := 1
    houses := map[Position]bool{}
    pos: Position

    for {
        c := bufio.reader_read_byte(&r) or_break

        if c == '^' {
            pos.y += 1
        } else if c == 'v' {
            pos.y -= 1
        } else if c == '<' {
            pos.x -= 1
        } else if c == '>' {
            pos.x += 1
        }

        if pos in houses {
            continue
        }

        houses[pos] = true
        numHouses += 1;
    }

    fmt.println(numHouses)
}
