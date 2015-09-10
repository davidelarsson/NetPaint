#ifndef SERVER_H
#define SERVER_H
#include "network.h"
#include "ncurses.h"

#define MAX_CANVAS_HEIGHT 10000
#define MAX_CANVAS_WIDTH 10000

int main(int argc, char* argv[]);
void handle_sigwinch(int sig);
int do_server(char* address, int port);
int event_loop(connections_t *connections, repaint_context_t *rc);
void log_errors(connections_t *con, repaint_context_t *rc);

#endif /* SERVER_H */
