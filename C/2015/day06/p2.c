#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(a) (sizeof((a)) / sizeof(*(a)))

#define GRID_WIDTH  1000
#define GRID_HEIGHT 1000

uint16_t grid[GRID_WIDTH * GRID_HEIGHT];

static void turnOn(uint32_t x, uint32_t y) {
    grid[y * GRID_WIDTH + x]++;
}

static void turnOff(uint32_t x, uint32_t y) {
    if (grid[y * GRID_WIDTH + x] > 0) {
        grid[y * GRID_WIDTH + x]--;
    }
}

static void toggle(uint32_t x, uint32_t y) {
    grid[y * GRID_WIDTH + x] += 2;
}

typedef struct {
    size_t size;
    size_t capacity;
} VecHeader;

static VecHeader *VecGetHeader(void *vec) {
    if (vec == NULL) {
        return NULL;
    }

    return &((VecHeader *)vec)[-1];
}

#define VecInit(vec, capacity_) do { \
    (vec) = malloc(sizeof(VecHeader) + capacity_ * sizeof(*(vec))); \
    memset((vec), 0, sizeof(VecHeader) + capacity_ * sizeof(*(vec))); \
    VecHeader *header = (VecHeader *)(vec); \
    header->capacity = capacity_; \
    (vec) = (void *)(&header[1]); \
} while (0)

#define VecUninit(vec) do { \
    free(VecGetHeader((vec))); \
    (vec) = NULL; \
} while (0)

static size_t VecGetCapacity(void *vec) {
    if (vec == NULL) {
        return 0;
    }

    return VecGetHeader(vec)->capacity;
}

static size_t VecGetSize(void *vec) {
    if (vec == NULL) {
        return 0;
    }

    return VecGetHeader(vec)->size;
}

#define VecPush(vec, item) do { \
    if ((vec) == NULL) { \
        VecInit((vec), 8); \
    } \
    VecHeader *header = VecGetHeader((vec)); \
    if (header->size == header->capacity) { \
        header->capacity *= 2; \
        header = realloc(header, sizeof(VecHeader) + header->capacity * sizeof(*(vec))); \
        (vec) = (void *)(&header[1]); \
    } \
    (vec)[header->size] = (item); \
    header->size++; \
} while (0)

typedef struct {
    size_t len;
    size_t capacity;
    char *s;
} StringBuilder;

#define SB_Fmt "%.*s"
#define SB_Arg(sb) (int)(sb).len, (sb).s

#define SB_C(str) ( \
    (StringBuilder) { \
        .len = sizeof((str)) - 1, \
        .s = (char [sizeof((str)) - 1]) {(str)} \
    } \
)

static void SBinit(StringBuilder *sb, size_t capacity) {
    sb->len = 0;
    sb->capacity = capacity;
    sb->s = malloc(capacity * sizeof(*sb->s));
}

static void SBuninit(StringBuilder *sb) {
    free(sb->s);
    sb->len = 0;
    sb->capacity = 0;
    sb->s = NULL;
}

#define SBpush(sb, c) do { \
    if ((sb).s == NULL) { \
        SBinit(&sb, 8); \
    } \
    if ((sb).len == (sb).capacity) { \
        (sb).capacity *= 2; \
        (sb).s = realloc((sb).s, (sb).capacity * sizeof(*(sb).s)); \
    } \
    (sb).s[(sb).len] = (c); \
    (sb).len++; \
} while (0)

static bool SBeq(const StringBuilder *a, const StringBuilder *b){
    if (a->len != b->len) {
        return false;
    }

    return memcmp(a, b, a->len) == 0;
}

static long SBtol(const StringBuilder sb) {
    long res = 0;

    for (size_t i = 0; i < sb.len; i++) {
        res = (res * 10) + (sb.s[i] - '0');
    }

    return res;
}

typedef struct {
    size_t len;
    const char *s;
} StringView;

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)(sv).len, (sv).s

static StringView SVreadEntireFile(const char *filepath) {
    FILE *f = fopen(filepath, "r");

    if (f == NULL) {
        return (StringView) { 0 };
    }

    StringView res;

    fseek(f, 0, SEEK_END);
    res.len = ftell(f);
    fseek(f, 0, SEEK_SET);

    res.s = malloc(res.len * sizeof(*res.s));

    if (res.s == NULL) {
        fclose(f);
        return (StringView) { 0 };
    }

    fread((void *)res.s, sizeof(*res.s), res.len, f);

    fclose(f);
    return res;
}

typedef enum {
    TOKEN_ALPHA,
    TOKEN_NUMBER,
    TOKEN_COMMA
} TokenType;

typedef struct {
    TokenType type;
    StringBuilder value;
} Token;

static Token *tokenize(StringView sv) {
    Token *tokens = NULL;

    for (size_t i = 0; i < sv.len; i++) {
        char c = sv.s[i];

        if (isspace(c)) {
            continue;
        }

        Token t = { 0 };

        if (isalpha(c)) {
            t.type = TOKEN_ALPHA;
            do {
                SBpush(t.value, c);

                if (i + 1 >= sv.len
                ||  !isalpha(sv.s[i + 1])) {
                    break;
                }

                i++;
                c = sv.s[i];
            } while (true);
        } else if (isdigit(c)) {
            t.type = TOKEN_NUMBER;
            do {
                SBpush(t.value, c);

                if (i + 1 >= sv.len
                ||  !isdigit(sv.s[i + 1])) {
                    break;
                }

                i++;
                c = sv.s[i];
            } while (true);
        } else if (c == ',') {
            t.type = TOKEN_COMMA;
            SBpush(t.value, ',');
        } else {
            fprintf(stderr, "Error while tokenizing. '%c' is unknown\n", c);
            exit(EXIT_FAILURE);
        }

        VecPush(tokens, t);
    }

    return tokens;
}

typedef enum {
    STAGE_NONE,
    STAGE_1ST_NUM_PAIR,
    STAGE_2ND_NUM_PAIR,
    STAGE_EXECUTE,
} Stage;

typedef enum {
    ACTION_TOGGLE,
    ACTION_TURN_OFF,
    ACTION_TURN_ON,
} Action;

static void parse(Token *tokens) {
    Stage s = STAGE_NONE;
    Action a;
    size_t x1, y1;
    size_t x2, y2;

    size_t i = 0;
    while (i < VecGetSize(tokens)) {
        Token t = tokens[i];

        switch (s) {
            case STAGE_NONE: {
                if (t.type != TOKEN_ALPHA) {
                    fprintf(stderr, "Expected ALPHA, got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                if (SBeq(&t.value, &SB_C("toggle"))) {
                    a = ACTION_TOGGLE;
                    s = STAGE_1ST_NUM_PAIR;

                    i++;
                    continue;
                }

                if (!SBeq(&t.value, &SB_C("turn"))) {
                    fprintf(stderr, "Expected 'turn', got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                i++;
                t = tokens[i];

                if (SBeq(&t.value, &SB_C("on"))) {
                    a = ACTION_TURN_ON;
                } else if (SBeq(&t.value, &SB_C("off"))) {
                    a = ACTION_TURN_OFF;
                } else {
                    fprintf(stderr, "Expected 'off' or 'on', got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                s = STAGE_1ST_NUM_PAIR;
            } break;
            case STAGE_1ST_NUM_PAIR: {
                if (t.type != TOKEN_NUMBER) {
                    fprintf(stderr, "Expected NUMBER, got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                x1 = SBtol(t.value);

                i++;
                t = tokens[i];

                if (t.type != TOKEN_COMMA) {
                    fprintf(stderr, "Expected COMMA, got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                i++;
                t = tokens[i];

                if (t.type != TOKEN_NUMBER) {
                    fprintf(stderr, "Expected NUMBER, got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                y1 = SBtol(t.value);
                s = STAGE_2ND_NUM_PAIR;
            } break;
            case STAGE_2ND_NUM_PAIR: {
                if (t.type != TOKEN_ALPHA
                ||  !SBeq(&t.value, &SB_C("through"))) {
                    fprintf(stderr, "Expected 'through', got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                i++;
                t = tokens[i];

                if (t.type != TOKEN_NUMBER) {
                    fprintf(stderr, "Expected NUMBER, got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                x2 = SBtol(t.value);

                i++;
                t = tokens[i];

                if (t.type != TOKEN_COMMA) {
                    fprintf(stderr, "Expected COMMA, got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                i++;
                t = tokens[i];

                if (t.type != TOKEN_NUMBER) {
                    fprintf(stderr, "Expected NUMBER, got '"SB_Fmt"'\n", SB_Arg(t.value));
                    exit(EXIT_FAILURE);
                }

                y2 = SBtol(t.value);
                s = STAGE_EXECUTE;

                continue;
            } break;
            case STAGE_EXECUTE: {
                for (size_t y = y1; y <= y2; y++) {
                    for (size_t x = x1; x <= x2; x++) {
                        switch (a) {
                            case ACTION_TOGGLE: {
                                toggle(x, y);
                            } break;
                            case ACTION_TURN_OFF: {
                                turnOff(x, y);
                            } break;
                            case ACTION_TURN_ON: {
                                turnOn(x, y);
                            } break;
                        }
                    }
                }

                s = STAGE_NONE;
            } break;
        }

        i++;
    }
}

static void cleanupTokens(Token **tokens) {
    Token *t = *tokens;

    for (size_t i = 0; i < VecGetSize(t); i++) {
        SBuninit(&t->value);
    }

    VecUninit(t);
    *tokens = NULL;
}

int main() {
    StringView file = SVreadEntireFile("input");

    if (file.s == NULL) {
        fprintf(stderr, "Failed to open input\n");
    }

    Token *tokens = tokenize(file);

    parse(tokens);

    cleanupTokens(&tokens);
    free((void *)file.s);

    uint64_t totalBrightness = 0;
    for (size_t i = 0; i < ARRAY_LEN(grid); i++) {
        totalBrightness += grid[i];
    }

    printf("%lu\n", totalBrightness);

    return EXIT_SUCCESS;
}
