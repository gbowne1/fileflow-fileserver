#include "util.h"

bool is_safe_path(const char *base_dir, const char *user_input, char *resolved_path, size_t path_size) {
    char temp_path[PATH_MAX];

    // Reject dangerous characters (extra precaution)
    for (const char *p = user_input; *p; ++p) {
        if (!isalnum(*p) && *p != '.' && *p != '-' && *p != '_' && *p != '/') {
            return false;
        }
    }

    // Join base_dir and user_input
    snprintf(temp_path, sizeof(temp_path), "%s/%s", base_dir, user_input);

    // Canonicalize the full path
    if (realpath(temp_path, resolved_path) == NULL) {
        return false; // File doesn't exist or is invalid
    }

    // Make sure resolved_path starts with realpath(PUBLIC_FOLDER)
    char public_dir[PATH_MAX];
    if (realpath(base_dir, public_dir) == NULL) {
        return false;
    }

    if (strncmp(resolved_path, public_dir, strlen(public_dir)) != 0) {
        return false; // Path escapes PUBLIC_FOLDER
    }

    // Optional: Reject symlinks
    struct stat st;
    if (lstat(resolved_path, &st) == 0 && S_ISLNK(st.st_mode)) {
        return false;
    }

    return true;
}
