#include "pwd.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <openssl/evp.h>

#define HASH_SIZE 32 // SHA256 produces 32 bytes

// Hash a password into a hex string buffer (assumed to be at least 2*HASH_SIZE+1)
void hash_password_sha256(const char *password, char *output_hex) {
    unsigned char hash[HASH_SIZE];
    unsigned int hash_len = 0;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create EVP_MD_CTX\n");
        return;
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
        EVP_DigestUpdate(ctx, password, strlen(password)) != 1 ||
        EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1) {
        fprintf(stderr, "EVP digest failed\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    EVP_MD_CTX_free(ctx);

    for (unsigned int i = 0; i < hash_len; i++) {
        sprintf(output_hex + (i * 2), "%02x", hash[i]);
    }
    output_hex[2 * hash_len] = '\0'; // Null terminate
}

bool verify_password(const char *input_password, const char *stored_hash) {
    char hash[2 * HASH_SIZE + 1];
    hash_password_sha256(input_password, hash);
    return strcmp(hash, stored_hash) == 0;
}

void hash_password(const char *password, char *hash) {
    hash_password_sha256(password, hash);
}
