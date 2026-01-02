#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct {
    size_t len;
    size_t capacity;
    char *s;
} StringBuilder;

static StringBuilder SBreadEntireFile(const char *filepath) {
    FILE *f = fopen(filepath, "r");

    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *s = malloc(n * sizeof(*s));
    fread(s, sizeof(*s), n, f);

    fclose(f);

    return (StringBuilder) {
        .len = n,
        .capacity = n,
        .s = s,
    };
}

typedef struct {
    size_t len;
    const char *s;
} StringView;

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)(sv).len, (sv).s

static StringView SVfromSB(StringBuilder sb) {
    return (StringView) {
        .len = sb.len,
        .s = sb.s,
    };
}

static StringView SVgetLine(StringView *sv) {
    size_t i = 0;
    while ( i <= sv->len && sv->s[i] != '\n') {
        i++;
    }

    StringView res = {
        .len = i,
        .s = sv->s,
    };

    sv->len -= i;
    sv->s += i;

    if (sv->len > 0) {
        sv->len--;
        sv->s++;
    }

    return res;
}

static long SVtol(StringView sv, StringView *endSv) {
    long res = 0;

    size_t i = 0;
    while (i <= sv.len && isdigit(sv.s[i])) {
        res = (res * 10) + (sv.s[i] - '0');
        i++;
    }

    if (endSv != NULL) {
        endSv->s += i;
        endSv->len -= i;
    }

    return res;
}

int main() {
    StringBuilder sb = SBreadEntireFile("input");
    StringView sv = SVfromSB(sb);

    if (sb.s == NULL) {
        fprintf(stderr, "Faild to open input");
        return EXIT_FAILURE;
    }

    int total = 0;

    while (sv.len > 0) {
        StringView line = SVgetLine(&sv);

        // printf(SV_Fmt"\n", SV_Arg(line));

        int dim[3] = { 0 };

        for (size_t i = 0; i < 3; i++) {
            dim[i] = SVtol(line, &line);

            if (line.len == 0) {
                break;
            }

            line.s++;
            line.len--;
        }

        int min = INT_MAX;

        for (size_t i = 0; i < 3; i++) {
            int area = dim[i] * dim[(i + 1) % 3];

            total += 2 * area;
            min = MIN(min, area);
        }

        total += min;
    }

    printf("Total sq ft: %d\n", total);

    free(sb.s);

    return EXIT_SUCCESS;
}
