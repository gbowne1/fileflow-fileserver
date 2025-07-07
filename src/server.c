
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
#include <limits.h>
#include <fcntl.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"

#define BUFFER_SIZE 1024
#define MAX_FILE_SIZE (10 * 1024 * 1024)
#define PUBLIC_FOLDER "public"

/* Function to determine the content type based on the file extension */
const char* get_content_type(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "application/octet-stream";

    if (strcmp(dot, ".html") == 0) return "text/html";
    if (strcmp(dot, ".txt") == 0) return "text/plain";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(dot, ".png") == 0) return "image/png";
    if (strcmp(dot, ".gif") == 0) return "image/gif";
    if (strcmp(dot, ".pdf") == 0) return "application/pdf";
    /* Add more types as needed */

    return "application/octet-stream";
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    /* Read the filename from the client */
    bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read from socket");
        close(client_socket);
        return;
    } else if (bytes_read == 0) {
        /* Client has closed the connection */
        fprintf(stderr, "Client disconnected unexpectedly\n");
        close(client_socket);
        return;
    }
    
    /* Ensure the buffer is null-terminated */
    buffer[bytes_read] = '\0'; /* Null-terminate the string */

    /* Check if the filename length exceeds the buffer size */
    if (bytes_read >= BUFFER_SIZE - 1) {
        fprintf(stderr, "Filename too long: %s\n", buffer);
        close(client_socket);
        return;
    }

    /* Sanitize the filename here because we can */
    if (strstr(buffer, "..") != NULL) {
        fprintf(stderr, "Invalid filename requested: %s\n", buffer);
        close(client_socket);
        return;
    }

    char *newline = strchr(buffer, '\n');
    if (newline) {
        *newline = '\0'; /* Replace newline with null terminator */
    }

    /* Open the requested file */
    FILE *file = fopen(buffer, "r");
    if (file == NULL) {
        perror("File not found");
        fprintf(stderr, "Requested file: %s\n", buffer);
        close(client_socket);
        return;
    }

     /* Determine the content type */
    const char* content_type = get_content_type(buffer);
    
    /* Send the content type header (for HTTP-like response) */
    char header[256];
    snprintf(header, sizeof(header), "Content-Type: %s\r\n\r\n", content_type);
    write(client_socket, header, strlen(header));

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET); /* Reset file pointer to the beginning */

    if (file_size > MAX_FILE_SIZE) {
        fprintf(stderr, "File size exceeds limit: %s (%ld bytes)\n", buffer, file_size);
        fclose(file);
        close(client_socket);
        return;
    }

    if (flock(fileno(file), LOCK_SH) != 0) {
        perror("Failed to lock file");
        fclose(file);
        close(client_socket);
        return;
    }

    /* Send the file content to the client */
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (write(client_socket, buffer, bytes_read) < 0) {
            perror("Failed to write to socket");
            flock(fileno(file), LOCK_UN);
            fclose(file);
            close(client_socket);
            return;
        }
    }

    /* Check for read errors */
    if (ferror(file)) {
        perror("Failed to read from file");
    }

    flock(fileno(file), LOCK_UN);
    fclose(file);
    close(client_socket);
}

int create_server_socket(int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    /* Set socket options to allow reuse of the address */
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Failed to set socket options");
        close(server_socket);
        return -1;
    }

    return server_socket;
}

void handle_connections(int server_socket) {
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) >= 0) {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)(intptr_t)client_socket);
        pthread_detach(thread_id); /* Detach the thread to allow it to clean up after itself */
    }
}
