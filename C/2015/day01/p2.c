#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *f = fopen("input", "r");

    if (f == NULL) {
        fprintf(stderr, "Failed to open input\n");
        return EXIT_FAILURE;
    }

    int floor = 0;

    while (true) {
        int c = fgetc(f);

        if (c == EOF) {
            break;
        }

        if (c == '(') {
            floor++;
        } else if (c == ')') {
            floor--;
        }

        if (floor == -1) {
            printf("Pos: %ld\n", ftell(f));
            break;
        }
    }

    fclose(f);

    return EXIT_SUCCESS;
}
