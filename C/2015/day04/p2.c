#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>

int main() {
    const char *input = "yzbqklnj";
    uint64_t num = 0;

    char buf[256];

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    const EVP_MD *type = EVP_md5();

    while (true) {
        EVP_DigestInit(ctx, type);

        snprintf(buf, sizeof(buf), "%s%lu", input, num);

        EVP_DigestUpdate(ctx, buf, strlen(buf));
        EVP_DigestFinal(ctx, hash, &hashLen);

        if (hash[0] == 0
        &&  hash[1] == 0
        &&  hash[2] == 0) {
            break;
        }

        num++;
    }

    EVP_MD_CTX_free(ctx);

    printf("%lu\n", num);

    return EXIT_SUCCESS;
}
