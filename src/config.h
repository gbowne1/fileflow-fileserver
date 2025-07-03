#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int port;
    char log_file[256];
} Config;

void load_config(const char *filename, Config *config);

#endif // CONFIG_H
