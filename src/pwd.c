#include "pwd.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define HASH_SIZE 128

// Simple hash function (not secure, for demonstration only)
void simple_hash(const char *password, char *hash) {
    unsigned int i;
    unsigned char temp = 0;

    for (i = 0; i < strlen(password); i++) {
        temp ^= password[i]; // Simple XOR operation
    }

    snprintf(hash, HASH_SIZE, "%02x", temp); // Store the hash as a hex string
}

// Verify the password by hashing the input and comparing it to the stored hash
bool verify_password(const char *input_password, const char *stored_hash) {
    char hash[HASH_SIZE];
    simple_hash(input_password, hash);
    return strcmp(hash, stored_hash) == 0;
}

// Hash the password and store it in the provided hash buffer
void hash_password(const char *password, char *hash) {
    simple_hash(password, hash);
}
