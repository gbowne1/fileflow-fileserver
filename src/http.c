#include "http.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int handle_http_request(int client_socket) {
    char buffer[2048];
    int received = read(client_socket, buffer, sizeof(buffer) - 1);
    if (received <= 0) return -1;

    buffer[received] = '\0';

    if (strncmp(buffer, "GET", 3) != 0) {
        const char *msg = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        write(client_socket, msg, strlen(msg));
        return 0;
    }

    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Welcome to the FILE BBS Server (HTTP Mode)\n";
    
    write(client_socket, response, strlen(response));
    return 0;
}
