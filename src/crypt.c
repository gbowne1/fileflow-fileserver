#include "crypt.h"
#include <string.h>
#include <stdio.h>

void encrypt_data(const char *data, char *encrypted_data, size_t data_size, size_t encrypted_data_size) {
    if (data_size > encrypted_data_size) {
        // Handle error: input data is too large for the output buffer
        fprintf(stderr, "Error: Input data size exceeds the allocated size for encrypted data.\n");
        return;
    }

    for (size_t i = 0; i < data_size; i++) {
        encrypted_data[i] = data[i] ^ 0xFF; // XOR encryption
    }
    encrypted_data[data_size] = '\0'; // Null-terminate
}

void decrypt_data(const char *encrypted_data, char *data, size_t encrypted_data_size, size_t data_size) {
    if (encrypted_data_size > data_size) {
        // Handle error: input encrypted data is too large for the output buffer
        fprintf(stderr, "Error: Encrypted data size exceeds the allocated size for decrypted data.\n");
        return;
    }

    for (size_t i = 0; i < encrypted_data_size; i++) {
        data[i] = encrypted_data[i] ^ 0xFF; // XOR decryption
    }
    data[encrypted_data_size] = '\0'; // Null-terminate
}
