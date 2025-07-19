#include "log.h"
#include <stdio.h>
#include <pthread.h>
#include <time.h>

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex;
static int logging_initialized = 0;

void init_logging(const char *filename) {
    if (logging_initialized) {
        fprintf(stderr, "Logging is already initialized.\n");
        return;
    }

    log_file = fopen(filename, "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }

    pthread_mutex_init(&log_mutex, NULL);
    logging_initialized = 1;
}

void log_message(const char *message) {
    pthread_mutex_lock(&log_mutex);
    if (log_file) {
        time_t now = time(NULL);
        fprintf(log_file, "[%s] %s\n", ctime(&now), message);
        fflush(log_file);
    }
    pthread_mutex_unlock(&log_mutex);
}

void close_logging() {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
        logging_initialized = 0;
    }
    pthread_mutex_destroy(&log_mutex);
}
