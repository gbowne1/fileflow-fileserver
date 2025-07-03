#include "log.h"
#include <stdio.h>
#include <pthread.h>
#include <time.h>

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex;

void init_logging(const char *filename) {
    log_file = fopen(filename, "a");
    pthread_mutex_init(&log_mutex, NULL);
}

void log_message(const char *message) {
    pthread_mutex_lock(&log_mutex);
    if (log_file) {
        time_t now = time(NULL);
        fprintf(log_file, "[%s] %s\n", ctime(&now), message);
        fflush(log_file); // Ensure the message is written immediately
    }
    pthread_mutex_unlock(&log_mutex);
}

void close_logging() {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
    pthread_mutex_destroy(&log_mutex);
}
