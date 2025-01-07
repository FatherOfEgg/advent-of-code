#+private file
package d05

import "core:fmt"
import "core:os"
import "core:bufio"
import "core:strings"
import "core:bytes"

INPUT_FILE :: "input.txt"

main :: proc() {
    f, fErr := os.open(INPUT_FILE)

    if fErr != nil {
        fmt.eprintfln("Couldn't open `%s`", INPUT_FILE)
        os.exit(1)
    }

    defer os.close(f)

    r: bufio.Reader
    bufio.reader_init(&r, os.stream_from_handle(f))
    defer bufio.reader_destroy(&r)

    niceStrs := 0

    for {
        line := bufio.reader_read_string(&r, '\n') or_break
        defer delete(line)

        pairs := map[string]bool{}
        pair := false
        prevPair := ""
        surround := false

        for i in 0..<len(line) - 1 {
            p := line[i:][:2]

            if p in pairs && p != prevPair {
                prevPair = ""
                pair = true
            }

            if p == prevPair {
                prevPair = ""
            } else {
                prevPair = p
            }

            pairs[p] = true

            if i > 0 {
                if line[i - 1] == line[i + 1] {
                    surround = true
                }
            }
        }

        if pair && surround {
            niceStrs += 1
        }
    }

    fmt.println(niceStrs)
}
