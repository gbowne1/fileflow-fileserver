#ifndef PWD_H
#define PWD_H

#include <stdbool.h>

bool verify_password(const char *input_password, const char *stored_hash);
void hash_password(const char *password, char *hash, const char *salt);

#endif // PWD_H
