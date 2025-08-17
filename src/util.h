#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

bool is_safe_path(const char *base, const char *user_input, char *resolved_path, size_t resolved_size);

#endif
