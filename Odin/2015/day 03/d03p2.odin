#+private file
package main

import "core:fmt"
import "core:os"
import "core:thread"
import "core:sync"

INPUT_FILE :: "input.txt"
N :: 2

Position :: struct {
    x, y : int
}

f: os.Handle
houses := map[Position]bool{
    {0, 0} = true
}
m: sync.Mutex
numHouses: [N]uint

main :: proc() {
    fErr: os.Error
    f, fErr = os.open(INPUT_FILE)

    if fErr != nil {
        assert(false, "Couldn't open: " + INPUT_FILE)
    }

    defer os.close(f)

    worker_proc :: proc(t: ^thread.Thread) {
        i := i64(t.user_index)
        b: [1]byte
        pos: Position

        for {
            n, err := os.read_at(f, b[:], i)

            // fmt.println(n, b[0], err, i)
            if n == 0 && err == nil {
                break
            }

            c := b[0]
            if c == '^' {
                pos.y += 1
            } else if c == 'v' {
                pos.y -= 1
            } else if c == '<' {
                pos.x -= 1
            } else if c == '>' {
                pos.x += 1
            }

            sync.mutex_lock(&m)

            if pos not_in houses {
                houses[pos] = true
                numHouses[t.user_index] += 1
            }

            sync.mutex_unlock(&m)

            i += 2
        }
    }

    threads := make([dynamic]^thread.Thread, 0, N)
    defer delete(threads)

    for i in 0..<N {
        t := thread.create(worker_proc)

        if t != nil {
            t.user_index = i
            append(&threads, t)
            thread.start(t)
        }
    }

    for len(threads) > 0 {
        i := 0

        for i < len(threads) {
            t := threads[i]

            if thread.is_done(t) {
                thread.destroy(t)
                ordered_remove(&threads, i)
            } else {
                i += 1
            }
        }
    }

    fmt.println(1 + numHouses[0] + numHouses[1])
}
