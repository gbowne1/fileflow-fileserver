#include "session.h"
#include <string.h>
#include <unistd.h>

void init_session(session_t *session, int client_socket, struct sockaddr_in client_addr) {
    session->client_socket = client_socket;
    session->client_addr = client_addr;
    session->active = true;
    session->current_menu = 0;
    memset(session->username, 0, sizeof(session->username));
}

void close_session(session_t *session) {
    if (session->active) {
        close(session->client_socket);
        session->active = false;
    }
}
