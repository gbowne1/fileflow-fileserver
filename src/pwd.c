#include "pwd.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <openssl/sha.h>

#define HASH_SIZE SHA256_DIGEST_LENGTH

// Hash a password into a hex string buffer (assumed to be at least 2*HASH_SIZE+1)
void hash_password_sha256(const char *password, char *output_hex) {
    unsigned char hash[HASH_SIZE];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Final(hash, &sha256);

    for (int i = 0; i < HASH_SIZE; i++) {
        sprintf(output_hex + (i * 2), "%02x", hash[i]);
    }
    output_hex[2 * HASH_SIZE] = '\0'; // Null terminate
}

// Verify the password by hashing the input and comparing it to the stored hash
bool verify_password(const char *input_password, const char *stored_hash) {
    char hash[HASH_SIZE];
    hash_password_sha256(input_password, hash);
    return strcmp(hash, stored_hash) == 0;
}

// Hash the password and store it in the provided hash buffer
void hash_password(const char *password, char *hash) {
    hash_password_sha256(password, hash);
}
