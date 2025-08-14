#include "tui.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void tui_clear_screen(session_t *session) {
    const char *clear = "\033[2J\033[H";
    send(session->client_socket, clear, strlen(clear), 0);
}

void tui_send(session_t *session, const char *text) {
    send(session->client_socket, text, strlen(text), 0);
}

void tui_send_line(session_t *session, const char *line) {
    tui_send(session, line);
    tui_send(session, "\r\n");
}

void tui_render_main_menu(session_t *session) {
    tui_clear_screen(session);
    tui_send_line(session, "╔════════════════════════════╗");
    tui_send_line(session, "║   FILE BBS TERMINAL MENU   ║");
    tui_send_line(session, "╠════════════════════════════╣");
    tui_send_line(session, "║ 1. List Files              ║");
    tui_send_line(session, "║ 2. About This Server       ║");
    tui_send_line(session, "║ 3. Quit                    ║");
    tui_send_line(session, "╚════════════════════════════╝");
    tui_send_line(session, "");
    tui_send(session, "> ");
}
