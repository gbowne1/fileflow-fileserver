#ifndef SESSION_H
#define SESSION_H

#include <netinet/in.h>
#include <stdbool.h>

typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
    bool active;
    int current_menu; // future: tracks which screen they're on
    char username[64]; // optional: could support login
} session_t;

void init_session(session_t *session, int client_socket, struct sockaddr_in client_addr);
void close_session(session_t *session);

#endif
