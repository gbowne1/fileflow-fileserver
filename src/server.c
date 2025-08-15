
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
#include "session.h"
#include "tui.h"

#define BUFFER_SIZE 1024
#define MAX_FILE_SIZE (10 * 1024 * 1024) // 10 MB
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
    struct sockaddr_in dummy_addr;
    session_t session;
    init_session(&session, client_socket, dummy_addr);

    tui_render_main_menu(&session);

    char input[BUFFER_SIZE];
    int len = read(client_socket, input, sizeof(input) - 1);
    if (len <= 0) {
        close_session(&session);
        return;
    }

    input[len] = '\0';

    if (input[0] == '1') {
        tui_clear_screen(&session);
        tui_send_line(&session, "Enter the filename to download:");
        tui_send(&session, "> ");

        // Read filename
        len = read(client_socket, input, sizeof(input) - 1);
        if (len <= 0) {
            close_session(&session);
            return;
        }

        input[len] = '\0';

        // Trim newline
        char *newline = strchr(input, '\n');
        if (newline) *newline = '\0';

        // Sanitize
        if (strlen(input) >= BUFFER_SIZE - 1 || strstr(input, "..") != NULL) {
            tui_send_line(&session, "Invalid filename.");
            close_session(&session);
            return;
        }

        // Open file
        FILE *file = fopen(input, "r");
        if (!file) {
            tui_send_line(&session, "File not found.");
            close_session(&session);
            return;
        }

        // Check file size
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (file_size > MAX_FILE_SIZE) {
            tui_send_line(&session, "File too large.");
            fclose(file);
            close_session(&session);
            return;
        }

        // Lock file
        if (flock(fileno(file), LOCK_SH | LOCK_NB) != 0) {
            tui_send_line(&session, "File is currently locked.");
            fclose(file);
            close_session(&session);
            return;
        }

        // Send header
        const char* content_type = get_content_type(input);
        char header[256];
        snprintf(header, sizeof(header), "\r\n[Content-Type: %s]\r\n\r\n", content_type);
        tui_send(&session, header);

        // Send file content
        while ((len = fread(input, 1, sizeof(input), file)) > 0) {
            if (write(session.client_socket, input, len) < 0) {
                perror("Write failed");
                break;
            }
        }

        if (ferror(file)) {
            tui_send_line(&session, "\nError reading file.");
        }

        // Unlock + close
        flock(fileno(file), LOCK_UN);
        fclose(file);
        tui_send_line(&session, "\n\n[Transfer complete]");

    } else if (input[0] == '2') {
        tui_clear_screen(&session);
        tui_send_line(&session, "FILE BBS Server v0.1");
        tui_send_line(&session, "Built with C, ANSI, and dreams.");
    } else {
        tui_send_line(&session, "Goodbye!");
    }

    close_session(&session);
}

int create_server_socket(int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Allow address reuse
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Failed to set socket options");
        close(server_socket);
        return -1;
    }

    // Bind to the specified port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections on any IP
    server_addr.sin_port = htons(port);       // Convert to network byte order

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return -1;
    }

    // Start listening
    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        return -1;
    }

    return server_socket;
}

