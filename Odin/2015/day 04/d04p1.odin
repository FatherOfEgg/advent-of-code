package d04

import "core:fmt"
import "core:os"
import "core:crypto/legacy/md5"
import "core:strings"
import "core:strconv"

main :: proc() {
    assert(len(os.args) == 2)

    c: md5.Context
    h: [md5.DIGEST_SIZE]byte

    counter := 1

    for {
        md5.reset(&c)
        md5.init(&c)

        s := fmt.tprintf("%s%d", os.args[1], counter)
        md5.update(&c, transmute([]byte)s)
        md5.final(&c, h[:], true)
        free_all(context.temp_allocator)

        x := (^u32be)(&h[0])^

        if x & 0xFFFFF000 == 0 {
            break
        }

        counter += 1
    }

    fmt.println(counter)
}
