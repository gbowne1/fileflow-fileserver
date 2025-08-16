/*********************************************************************
 * The Open Group Base Specifications Issue 6
 * IEEE Std 1003.1, 2004 Edition
 *
 *    An XSI-conforming application should ensure that the feature
 *    test macro _XOPEN_SOURCE is defined with the value 600 before
 *    inclusion of any header. This is needed to enable the
 *    functionality described in The _POSIX_C_SOURCE Feature Test
 *    Macro and in addition to enable the XSI extension.
 *
 *********************************************************************/
#if ! defined (_XOPEN_SOURCE)
#define _XOPEN_SOURCE 600
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#include "log.h"
#include "config.h"
#include "pwd.h"
#include "crypt.h"

#define DEFAULT_PORT 8080

volatile sig_atomic_t keep_running = true;
int server_socket;

void handle_signal(int signal) {
    (void)signal;
    keep_running = false;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handle_signal); /* Handle Ctrl+C */
    signal(SIGTERM, handle_signal); /* Handle termination signal */

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(struct sockaddr_in);

    int port = DEFAULT_PORT;

    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number. Using default port %d.\n", DEFAULT_PORT);
            port = DEFAULT_PORT;
        }
    }

    /* Create socket */
    int server_socket = create_server_socket(port);
    if (server_socket < 0) {
        fprintf(stderr, "Failed to start server on port %d\n", port);
        exit(EXIT_FAILURE);
    }

    while (keep_running) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            if (keep_running) { /* Only print error if we are still running */
                perror("Accept failed");
            }
            continue; /* Continue to the next iteration if not shutting down */
        }
        handle_client(client_socket);
    }

    /* Cleanup: close the server socket before exiting */
    close(server_socket);
    printf("Server shutting down...\n");
    return 0;
}
