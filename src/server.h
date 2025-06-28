
#ifndef SERVER_H
#define SERVER_H

#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 600
#endif

void handle_client(int client_socket);
int create_server_socket(int port);
void handle_connections(int server_socket);

#endif // SERVER_H
