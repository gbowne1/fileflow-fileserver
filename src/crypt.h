#ifndef CRYPT_H
#define CRYPT_H

#include <stddef.h>

void encrypt_data(const char *data, char *encrypted_data, size_t data_size, size_t encrypted_data_size);
void decrypt_data(const char *encrypted_data, char *data, size_t encrypted_data_size, size_t data_size);

#endif // CRYPT_H
