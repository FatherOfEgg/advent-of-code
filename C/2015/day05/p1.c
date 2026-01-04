#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(a) (sizeof((a)) / sizeof(*(a)))

static bool isVowel(char c) {
    switch (c) {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
            return true;
        default:
            return false;
    }
}

typedef struct {
    size_t len;
    const char *s;
} StringView;

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)(sv).len, (sv).s

#define SV_C(s_) ( \
    (StringView) { \
        .len = sizeof((s_)) - 1, \
        .s = (const char[sizeof((s_)) - 1]){ s_ }, \
    } \
)

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
        return (StringView) {
            .len = 0,
            .s = NULL,
        };
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

static bool hasBadStrs(StringView sv) {
    const StringView badStrs[] = {
        SV_C("ab"),
        SV_C("cd"),
        SV_C("pq"),
        SV_C("xy"),
    };

    for (size_t i = 0; i < ARRAY_LEN(badStrs); i++) {
        if (SVeq(sv, badStrs[i])) {
            return true;
        }
    }

    return false;
}

static bool isNiceStr(StringView sv) {
    uint32_t vowels = 0;
    bool hasDouble = false;

    for (size_t i = 0; i < sv.len - 1; i++) {
        StringView window = {
            .len = 2,
            .s = sv.s + i,
        };

        if (hasBadStrs(window)) {
            return false;
        }

        if (isVowel(window.s[0])) {
            vowels++;
        }

        if (window.s[0] == window.s[1]) {
            hasDouble = true;
        }
    }

    if (isVowel(sv.s[sv.len - 1])) {
        vowels++;
    }

    return vowels >= 3 && hasDouble;
}

int main() {
    StringView file = SVreadEntireFile("input");

    if (file.s == NULL) {
        fprintf(stderr, "Failed to open input\n");
        return EXIT_FAILURE;
    }

    StringView sv = file;

    uint64_t num = 0;

    while (sv.len > 0) {
        StringView line = SVgetLine(&sv);

        if (isNiceStr(line)) {
            // printf(SV_Fmt"\n", SV_Arg(line));
            num++;
        }
    }

    printf("%lu\n", num);

    free((char *)file.s);

    return EXIT_SUCCESS;
}
