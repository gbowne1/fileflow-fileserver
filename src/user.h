#ifndef USER_H
#define USER_H

#include <stdbool.h>

#define MAX_USERNAME_LEN 64
#define MAX_PASSWORD_LEN 128
#define HASHED_PASSWORD_LEN 65  // 64 hex + null terminator
#define USER_FILE "users.txt"

typedef struct {
    char username[MAX_USERNAME_LEN];
    char hashed_password[HASHED_PASSWORD_LEN];
} User;

bool load_user(const char *username, User *user_out);
bool register_user(const char *username, const char *password);
bool authenticate_user(const char *username, const char *password);

#endif // USER_H
