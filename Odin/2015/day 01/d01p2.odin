#+private file
package d01

import "core:fmt"
import "core:os"
import "core:bufio"

INPUT_FILE :: "input.txt"

main :: proc() {
    f, fErr := os.open(INPUT_FILE)

    if fErr != nil {
        assert(false, "Couldn't open: " + INPUT_FILE)
    }

    defer os.close(f)


    r: bufio.Reader
    bufio.reader_init(&r, os.stream_from_handle(f))
    defer bufio.reader_destroy(&r)

    floor := 0
    i := 1

    for {
        c := bufio.reader_read_byte(&r) or_break

        if c == '(' {
            floor += 1
        } else if c == ')' {
            floor -= 1
        }

        if floor == -1 {
            break
        }

        i += 1
    }

    fmt.println(i)
}
