#include "user.h"
#include "pwd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load user from users.txt
bool load_user(const char *username, User *user_out) {
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) return false;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char file_username[MAX_USERNAME_LEN];
        char file_hash[HASHED_PASSWORD_LEN];

        if (sscanf(line, "%63[^:]:%64s", file_username, file_hash) == 2) {
            if (strcmp(file_username, username) == 0) {
                strncpy(user_out->username, file_username, MAX_USERNAME_LEN);
                strncpy(user_out->hashed_password, file_hash, HASHED_PASSWORD_LEN);
                fclose(fp);
                return true;
            }
        }
    }

    fclose(fp);
    return false;
}

// Register user and save to file
bool register_user(const char *username, const char *password) {
    User existing;
    if (load_user(username, &existing)) {
        return false; // User already exists
    }

    char hash[HASHED_PASSWORD_LEN];
    hash_password(password, hash);

    FILE *fp = fopen(USER_FILE, "a");
    if (!fp) return false;

    fprintf(fp, "%s:%s\n", username, hash);
    fclose(fp);
    return true;
}

// Authenticate user credentials
bool authenticate_user(const char *username, const char *password) {
    User user;
    if (!load_user(username, &user)) {
        return false;
    }

    return verify_password(password, user.hashed_password);
}
