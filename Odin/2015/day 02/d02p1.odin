#+private
package d02

import "core:fmt"
import "core:os"
import "core:bufio"
import "core:strings"
import "core:strconv"

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

    total := 0

    for {
        line := bufio.reader_read_string(&r, '\n') or_break
        defer delete(line)

        numStrs := strings.split(line, "x")
        defer delete(numStrs)

        dims: [3]int

        for numStr, i in numStrs {
            dims[i] = strconv.atoi(numStr)
        }

        a := dims[0] * dims[1]
        b := dims[1] * dims[2]
        c := dims[2] * dims[0]

        m := min(a, b, c)

        total += 2 * a + 2 * b + 2 * c + m
    }

    fmt.println(total)
}
