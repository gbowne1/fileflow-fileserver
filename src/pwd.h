#ifndef PWD_H
#define PWD_H

#include <stdbool.h>

// Function prototypes
void simple_hash(const char *password, char *hash);
bool verify_password(const char *input_password, const char *stored_hash);
void hash_password(const char *password, char *hash);

#endif // PWD_H
