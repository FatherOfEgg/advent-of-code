#+private file
package d05

import "core:fmt"
import "core:os"
import "core:bufio"
import "core:strings"
import "core:bytes"

INPUT_FILE :: "input.txt"
VOWELS :: []u8{'a', 'i', 'u', 'e', 'o'}
BAD_PARIS :: []string{"ab", "cd", "pq", "xy"}

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

        vowels := 0
        double := false
        badSubstr := false

        loop: for i in 0..<len(line) - 1 {
            for bad in BAD_PARIS {
                if strings.compare(line[i:][:2], bad) == 0 {
                    badSubstr = true
                    break loop
                }
            }

            if line[i] == line[i + 1] {
                double = true
            }

            if strings.contains_any(line[i:][:1], "aiueo") {
                vowels += 1
            }
        }

        if !badSubstr && double && vowels >= 3 {
            niceStrs += 1
        }
    }

    fmt.println(niceStrs)
}
