
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
#include "socket.h"
#include "server.h"
#include "session.h"
#include "tui.h"
#include "user.h"
#include "pwd.h"
#include "util.h"

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

    char input[BUFFER_SIZE];
    int len;

    // Step 1: Authentication Menu
    tui_clear_screen(&session);
    tui_send_line(&session, "FILE BBS Server v0.1");
    tui_send_line(&session, "Built with C, ANSI, and dreams.\n");
    tui_send_line(&session, "1. Login");
    tui_send_line(&session, "2. Register");
    tui_send_line(&session, "3. Quit");
    tui_send(&session, "\nSelect an option: ");

    len = read(client_socket, input, sizeof(input) - 1);
    if (len <= 0) {
        close_session(&session);
        return;
    }
    input[len] = '\0';

    if (input[0] == '3') {
        tui_send_line(&session, "Goodbye!");
        close_session(&session);
        return;
    }

    // Step 2: Get username
    tui_send_line(&session, "\nEnter username:");
    tui_send(&session, "> ");
    len = read(client_socket, input, sizeof(input) - 1);
    if (len <= 0) {
        close_session(&session);
        return;
    }
    input[len] = '\0';
    char *newline = strchr(input, '\n');
    if (newline) *newline = '\0';
    char username[MAX_USERNAME_LEN];
    strncpy(username, input, MAX_USERNAME_LEN);
    username[MAX_USERNAME_LEN - 1] = '\0';

    // Step 3: Get password
    tui_send_line(&session, "Enter password:");
    tui_send(&session, "> ");
    len = read(client_socket, input, sizeof(input) - 1);
    if (len <= 0) {
        close_session(&session);
        return;
    }
    input[len] = '\0';
    newline = strchr(input, '\n');
    if (newline) *newline = '\0';
    char password[MAX_PASSWORD_LEN];
    strncpy(password, input, MAX_PASSWORD_LEN);
    password[MAX_PASSWORD_LEN - 1] = '\0';

    // Step 4: Authenticate or register
    bool authenticated = false;
    if (input[0] == '1') {
        authenticated = authenticate_user(username, password);
        if (!authenticated) {
            tui_send_line(&session, "Authentication failed.");
            close_session(&session);
            return;
        }
        tui_send_line(&session, "Login successful.\n");
    } else if (input[0] == '2') {
        if (register_user(username, password)) {
            tui_send_line(&session, "Registration successful. You are now logged in.\n");
            authenticated = true;
        } else {
            tui_send_line(&session, "Username already exists. Try logging in.\n");
            close_session(&session);
            return;
        }
    } else {
        tui_send_line(&session, "Invalid selection.");
        close_session(&session);
        return;
    }

    if (!authenticated) {
        close_session(&session);
        return;
    }

    // Step 5: Show main menu
    tui_render_main_menu(&session);

    len = read(client_socket, input, sizeof(input) - 1);
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
        tui_send_line(&session, "Enter filename to upload:");
        tui_send(&session, "> ");

        // Read filename
        int len = read(session.client_socket, input, sizeof(input) - 1);
        if (len <= 0) {
            close_session(&session);
            return;
        }
        input[len] = '\0';
        char *newline = strchr(input, '\n');
        if (newline) *newline = '\0';

        // Sanitize and get absolute path
        char full_path[PATH_MAX];
        if (!is_safe_path(PUBLIC_FOLDER, input, full_path, sizeof(full_path))) {
            tui_send_line(&session, "Invalid or unsafe filename.");
            close_session(&session);
            return;
        }

        // Prompt for file size
        tui_send_line(&session, "Enter file size in bytes:");
        tui_send(&session, "> ");
        char size_buf[64];
        len = read(session.client_socket, size_buf, sizeof(size_buf) - 1);
        if (len <= 0) {
            close_session(&session);
            return;
        }
        size_buf[len] = '\0';
        long file_size = strtol(size_buf, NULL, 10);
        if (file_size <= 0 || file_size > MAX_FILE_SIZE) {
            tui_send_line(&session, "Invalid or too large file size.");
            close_session(&session);
            return;
        }

        // Open file for writing
        FILE *file = fopen(full_path, "wb");
        if (!file) {
            tui_send_line(&session, "Failed to open file for writing.");
            close_session(&session);
            return;
        }

        // Read and write file contents
        long bytes_remaining = file_size;
        while (bytes_remaining > 0) {
            char buf[1024];
            int chunk = (bytes_remaining > sizeof(buf)) ? sizeof(buf) : bytes_remaining;
            int received = read(session.client_socket, buf, chunk);
            if (received <= 0) {
                tui_send_line(&session, "Connection lost during upload.");
                fclose(file);
                close_session(&session);
                return;
            }
            fwrite(buf, 1, received, file);
            bytes_remaining -= received;
        }

        fclose(file);
        tui_send_line(&session, "[Upload complete]");
    } else if (input[0] == '3') {
        tui_clear_screen(&session);
        tui_send_line(&session, "Exiting...");
        tui_send_line(&session, "Thank you for using FILE BBS Server!");   
        tui_send_line(&session, "Goodbye!");
    }

    close_session(&session);
}
