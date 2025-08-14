#ifndef TUI_H
#define TUI_H

#include "session.h"

void tui_render_main_menu(session_t *session);
void tui_clear_screen(session_t *session);
void tui_send_line(session_t *session, const char *line);
void tui_send(session_t *session, const char *text);

#endif
