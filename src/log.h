#ifndef LOG_H
#define LOG_H

void init_logging(const char *filename);
void log_message(const char *message);
void close_logging();

#endif // LOG_H
