#ifndef NCURSES_H
#define NCURSES_H
#include <ncurses.h>
#include "ncurses.h"
#include "network.h"

#define MIN_SCREEN_WIDTH 50
#define MIN_SCREEN_HEIGHT 25
#define TOPWIN_HEIGHT 3
#define INFOWIN_HEIGHT 3

typedef enum state_s { NORMAL, HELP, CLIENT, ADJUST_CANVAS } state_t;

typedef struct log_str_s {
	char *str;
	struct log_str_t *next;
} log_str_t;

typedef struct repaint_context_s {
	int screen_width;
	int screen_height;
	int screen_size_ok; /* set to 0 if screen_width and screen_height have
			       values of least MIN_SCREEN_WIDTH and
			       MIN_SCREEN_HEIGHT respectively */
	state_t state;
	WINDOW *topwin;
	WINDOW *logwin;
	WINDOW *infowin;
	WINDOW *helpwin;
	WINDOW *clientwin;
	int active_client;
	log_str_t *head;
} repaint_context_t;


repaint_context_t *init_repaint_context();
void reinit_repaint_context(repaint_context_t *rc);
void adjust_canvas(connections_t *con, int key);
void toggle_write_lock(repaint_context_t *rc, connections_t *con);
void print_connections(repaint_context_t *rc, connections_t *con);
int handle_ui(repaint_context_t *rc, connections_t *connections);
void repaint(repaint_context_t *rc, connections_t *con);
void repaint_normal(repaint_context_t *rc, connections_t *con);
void repaint_help(repaint_context_t *rc);
void repaint_clients(repaint_context_t *rc, connections_t *con);
void repaint_adjust_canvas(repaint_context_t *rc, connections_t *con);
void resize_screen(repaint_context_t *rc);
void logstr(repaint_context_t *rc, char* str);

#endif /* NCURSES_H */
