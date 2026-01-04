#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(a) (sizeof((a)) / sizeof(*(a)))

typedef struct {
    size_t len;
    const char *s;
} StringView;

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)(sv).len, (sv).s

static bool SVeq(const StringView a, const StringView b) {
    if (a.len != b.len) {
        return false;
    }

    return memcmp(a.s, b.s, a.len) == 0;
}

static StringView SVreadEntireFile(const char *filepath) {
    FILE *f = fopen(filepath, "r");

    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *s = malloc(n);
    if (s == NULL) {
        return (StringView) { 0 };
    }

    fread(s, sizeof(*s), n, f);

    return (StringView) {
        .len = n,
        .s = s,
    };
}

static StringView SVgetLine(StringView *sv) {
    size_t i = 0;
    while (i <= sv->len && sv->s[i] != '\n') {
        i++;
    }

    StringView res = {
        .len = i,
        .s = sv->s,
    };

    sv->s += i;
    sv->len -= i;

    if (sv->len > 0) {
        sv->s++;
        sv->len--;
    }

    return res;
}

uint16_t hash(StringView sv) {
    return (sv.s[1] << 8) | sv.s[0];
}

typedef struct {
    size_t size;
    size_t capacity;
    StringView *items;
} Hashmap;

Hashmap hm;

static void HashmapInit(Hashmap *hm, size_t capacity) {
    hm->size = 0;
    hm->capacity = capacity;
    hm->items = calloc(capacity, sizeof(*hm->items));
}

static void HashmapUnint(Hashmap *hm) {
    hm->size = 0;
    hm->capacity = 0;
    free(hm->items);
    hm->items = NULL;
}

static void HashmapClear(Hashmap *hm) {
    hm->size = 0;
    memset(hm->items, 0, hm->capacity * sizeof(*hm->items));
}

static bool HashmapInsert(Hashmap *hm, StringView sv) {
    if (hm->size == hm->capacity) {
        hm->capacity *= 2;

        Hashmap temp;
        HashmapInit(&temp, hm->capacity);

        for (size_t i = 0; i < hm->size; i++) {
            HashmapInsert(&temp, hm->items[i]);
        }

        free(hm->items);
        hm->items = temp.items;
    }

    size_t index = hash(sv) & (hm->capacity - 1);

    while (hm->items[index].s != NULL) {
        if (SVeq(hm->items[index], sv)) {
            return false;
        }

        index = (index + 1) & (hm->capacity - 1);
    }

    hm->items[index] = sv;
    hm->size++;

    return true;
}

static bool isNiceStr(StringView sv) {
    HashmapClear(&hm);

    uint32_t pairs = 0;
    StringView lastPair = { 0 };
    bool hasRepeat = false;

    for (size_t i = 0; i < sv.len - 1; i++) {
        if (i < sv.len - 2) {
            if (sv.s[i] == sv.s[i + 2]) {
                hasRepeat = true;
            }
        }

        StringView window = {
            .len = 2,
            .s = sv.s + i,
        };

        if (!HashmapInsert(&hm, window)) {
            if (SVeq(window, lastPair)) {
                lastPair = (StringView) { 0 };
                continue;
            }

            pairs += 2;
        }

        lastPair = window;
    }

    return pairs >= 2 && hasRepeat;
}

int main() {
    StringView file = SVreadEntireFile("input");

    if (file.s == NULL) {
        fprintf(stderr, "Failed to open input\n");
        return EXIT_FAILURE;
    }

    HashmapInit(&hm, 256);

    StringView sv = file;

    uint64_t num = 0;

    while (sv.len > 0) {
        StringView line = SVgetLine(&sv);
        // printf(SV_Fmt"\n", SV_Arg(line));
        // printf("%lu\n", sv.len);

        if (isNiceStr(line)) {
            // printf(SV_Fmt"\n", SV_Arg(line));
            num++;
        }
    }

    printf("%lu\n", num);

    HashmapUnint(&hm);
    free((char *)file.s);

    return EXIT_SUCCESS;
}
