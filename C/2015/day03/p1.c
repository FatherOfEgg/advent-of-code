#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int x, y;
} Coord;

static bool CoordEq(Coord a, Coord b) {
    return a.x == b.x && a.y == b.y;
}

// https://stackoverflow.com/a/682485
static uint32_t hashCoord(Coord c) {
    return (c.x * 0x1f1f1f1f) ^ c.y;
}

typedef struct {
    Coord c;
    bool occupied;
} Value;

typedef struct {
    size_t size;
    size_t capacity;
    Value *items;
} Hashmap;

static void HashmapInit(Hashmap *hm, size_t capacity) {
    hm->size = 0;
    hm->capacity = capacity;

    hm->items = calloc(capacity, sizeof(*hm->items));
}

static void HashmapUnint(Hashmap *hm) {
    if (hm == NULL) {
        return;
    }

    free(hm->items);
    hm->items = NULL;
}

static bool HashmapInsert(Hashmap *hm, Coord c) {
    if (hm->size == hm->capacity) {
        hm->capacity *= 2;
        Hashmap temp = { 0 };

        HashmapInit(&temp, hm->capacity);

        for (size_t i = 0; i < hm->size; i++) {
            HashmapInsert(&temp, hm->items[i].c);
        }

        free(hm->items);
        hm->items = temp.items;
    }

    size_t index = hashCoord(c) & (hm->capacity - 1);

    while (hm->items[index].occupied) {
        if (CoordEq(hm->items[index].c, c)) {
            return false;
        }

        index = (index + 1) & (hm->capacity - 1);
    }

    hm->items[index] = (Value) {
        .c = c,
        .occupied = true,
    };
    hm->size++;

    return true;
}

int main() {
    Hashmap hm = { 0 };
    HashmapInit(&hm, 256);

    unsigned int total = 1;

    Coord pos = {
        .x = 0,
        .y = 0,
    };

    bool b = HashmapInsert(&hm, pos);

    FILE *f = fopen("input", "r");

    if (f == NULL) {
        fprintf(stderr, "Failed to open input\n");
        return EXIT_FAILURE;
    }

    while (true) {
        int c = fgetc(f);

        if (c == EOF) {
            break;
        }

        switch (c) {
            case '<': {
                pos.x--;
            } break;
            case '>': {
                pos.x++;
            } break;
            case '^': {
                pos.y++;
            } break;
            case 'v': {
                pos.y--;
            } break;
            default:
                break;
        }

        if(HashmapInsert(&hm, pos)) {
            total++;
        }
    }

    fclose(f);
    HashmapUnint(&hm);

    printf("Total: %u\n", total);

    return EXIT_SUCCESS;
}
