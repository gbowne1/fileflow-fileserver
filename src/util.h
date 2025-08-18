#ifndef UTIL_H
#define UTIL_H

#if ! defined (_XOPEN_SOURCE)
#define _XOPEN_SOURCE 600
#endif

#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

bool is_safe_path(const char *base, const char *user_input, char *resolved_path, size_t resolved_size);

#endif
