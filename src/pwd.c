
#include "pwd.h"
#include <string.h>
#include <crypt.h>
#include <stdio.h>

bool verify_password(const char *input_password, const char *stored_hash) {
    // Use the crypt function to hash the input password with the same salt
    char *hash = crypt(input_password, stored_hash);
    
    // Compare the hashed input password with the stored hash
    return strcmp(hash, stored_hash) == 0;
}

void hash_password(const char *password, char *hash, const char *salt) {
    // Use the crypt function to hash the password with the provided salt
    char *result = crypt(password, salt);
    
    // Copy the result to the provided hash buffer
    if (result != NULL) {
        strncpy(hash, result, 128); // Assuming hash buffer is large enough
        hash[127] = '\0'; // Ensure null termination
    } else {
        fprintf(stderr, "Error hashing password\n");
        hash[0] = '\0'; // Set hash to empty string on error
    }
}

