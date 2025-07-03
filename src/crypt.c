#include "crypt.h"
#include <string.h>

void encrypt_data(const char *data, char *encrypted_data) {
    // Simple encryption logic (for demonstration purposes)
    for (size_t i = 0; i < strlen(data); i++) {
        encrypted_data[i] = data[i] ^ 0xFF; // XOR encryption
    }
    encrypted_data[strlen(data)] = '\0'; // Null-terminate
}

void decrypt_data(const char *encrypted_data, char *data) {
    // Simple decryption logic (for demonstration purposes)
    for (size_t i = 0; i < strlen(encrypted_data); i++) {
        data[i] = encrypted_data[i] ^ 0xFF; // XOR decryption
    }
    data[strlen(encrypted_data)] = '\0'; // Null-terminate
}
