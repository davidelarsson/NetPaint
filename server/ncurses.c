#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <signal.h>
#include "ncurses.h"
#include "server.h"
#include "network.h"
#include "file.h"

int handle_ui(repaint_context_t *rc, connections_t *connections)
{
	(void) connections;

	int ch = getch();
	switch(rc->state) {
		case NORMAL:
			
			if(ch == 'q') /* ESC or q quits the server */
			       return -1; /* Tell caller we want to quit */
			else if(ch == 'h')
				rc->state = HELP;
			else if(ch == 'c')
				rc->state = CLIENT;
			else if(ch == 't')
				rc->state = ADJUST_CANVAS;
			else if(ch == 's')
			{
				save_file(connections);
				char str[100];
				sprintf(str, "Canvas saved to disk\n");
				logstr(rc, str);
			}
			break;

		case HELP:
			if(ch == 'b')
				rc->state = NORMAL;
			break;

		case CLIENT:
			if(ch == 'b')
				rc->state = NORMAL;
			else if(ch == KEY_UP)
			{
				if(rc->active_client > 0)
					rc->active_client -= 1;
			}
			else if(ch == KEY_DOWN)
			{
				if(rc->active_client < (int) connections->connections_n - 1)
				rc->active_client += 1;
			}
			else if(ch == ' ')
				toggle_write_lock(rc, connections);
			break;

		case ADJUST_CANVAS:
			if(ch == 'b')
			{
				rc->state = NORMAL;
				connections->canvas_w_tmp = connections->canvas_w;
				connections->canvas_h_tmp = connections->canvas_h;
			}
			else if(ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT
					|| ch == 't')
				adjust_canvas(connections, ch);
			if(ch == 't')
				rc->state = NORMAL;
			break;
	}
	return 0;
}

void adjust_canvas(connections_t *con, int key)
{
	if(key == KEY_DOWN && con->canvas_h_tmp > 100)
		con->canvas_h_tmp -= 10;
	else if(key == KEY_UP && con->canvas_h_tmp < MAX_CANVAS_HEIGHT)
		con->canvas_h_tmp += 10;
	else if(key == KEY_LEFT && con->canvas_w_tmp > 100)
		con->canvas_w_tmp -= 10;
	else if(key == KEY_RIGHT && con->canvas_w_tmp < MAX_CANVAS_WIDTH)
		con->canvas_w_tmp += 10;
	else if(key == 't')
		init_canvas(con);

}

void toggle_write_lock(repaint_context_t *rc, connections_t *connections)
{
	connection_t *con = connections->connection;
	int i = 0;
	while(con != NULL)
	{
		if(i == rc->active_client)
		{
			con->write_lock = ~con->write_lock;
			return;
		}
		con = (connection_t *) con->next;
		i++;
	}

}

void print_connections(repaint_context_t *rc, connections_t *connections)
{
	connection_t *con = connections->connection;
	while(con != NULL)
	{
		char str[100];
		sprintf(str, "connection %d: rec: %d, sent: %d\n",
				con->fd, con->rec_bytes, con->sent_bytes);
		logstr(rc, str);
		con = (connection_t *) con->next;
	}
}

/* This only needs to be called when the server starts and when the terminal is resized */
void reinit_repaint_context(repaint_context_t *rc)
{
	fprintf(stderr, "reinit!\n");
	endwin();
	refresh();
	initscr();
	clear();
	getmaxyx(stdscr, rc->screen_height, rc->screen_width);

	if(rc->screen_height < MIN_SCREEN_HEIGHT || rc->screen_width < MIN_SCREEN_WIDTH)
	{
		rc->screen_size_ok = 1;
	} else {
		rc->topwin = newwin(TOPWIN_HEIGHT, rc->screen_width, 0, 0);
		rc->infowin = newwin(INFOWIN_HEIGHT, rc->screen_width, TOPWIN_HEIGHT, 0);
		rc->logwin = newwin(rc->screen_height - (TOPWIN_HEIGHT + INFOWIN_HEIGHT),
				rc->screen_width, TOPWIN_HEIGHT + INFOWIN_HEIGHT, 0);
		rc->helpwin = newwin(rc->screen_height - TOPWIN_HEIGHT, rc->screen_width,
				TOPWIN_HEIGHT, 0);
		rc->clientwin = newwin(rc->screen_height - TOPWIN_HEIGHT, rc->screen_width,
				TOPWIN_HEIGHT, 0);
		rc->screen_size_ok = 0;
	}
}

repaint_context_t *init_repaint_context()
{
	/* ncurses init */
	initscr(); /* Initialize the screen */
	if(has_colors() == FALSE)
	{
		endwin();
		printf("Dude, seriously... no colors?! You're out!\n");
		exit(1);
	}
	start_color();
	cbreak(); /* No input buffering. FIXME: Use raw() instead? What's the difference? */
	ESCDELAY = 25; /* Oh God, why does this even exist? */
	noecho();
	curs_set(0); /* Turn off the cursor */
	keypad(stdscr, TRUE); /* Enable function and arrow keys input */
	repaint_context_t *rc = malloc(sizeof(repaint_context_t));
	reinit_repaint_context(rc);
	rc->head = NULL;
	rc->state = NORMAL;
	rc->active_client = 0;
	return rc;
}


void logstr(repaint_context_t *rc, char* tmp_str)
{
	log_str_t *line = malloc(sizeof(log_str_t));
	char *str = malloc(strlen(tmp_str) + 1);
	strcpy(str, tmp_str);
	line->next = (struct log_str_t *) rc->head;
	line->str = str;
	rc->head = line;
}


void repaint(repaint_context_t *rc, connections_t *con)
{
	/* If the screen is too small, we don't really paint anything */
	if(rc->screen_size_ok != 0)
	{
		box(stdscr, 0, 0);
		char *too_small_str = "Screen too small!";
		attron(A_BOLD);
		mvprintw(rc->screen_height / 2,
				rc->screen_width / 2 - strlen(too_small_str) / 2, too_small_str);
		attroff(A_BOLD);
		refresh();
		return;
	}

	clear();
	wclear(rc->topwin);

	/* Stuff that all states share */
	box(rc->topwin, 0, 0);
	char* top_title = " NetPaint Server 0.1 ";
	wattron(rc->topwin, A_BOLD);
	mvwprintw(rc->topwin, 0, rc->screen_width / 2 - strlen(top_title) / 2, top_title);
	wattroff(rc->topwin, A_BOLD);

	switch(rc->state) {
		case NORMAL:
			repaint_normal(rc, con);
			break;
		case HELP:
			repaint_help(rc);
			break;
		case CLIENT:
			repaint_clients(rc, con);
			break;
		case ADJUST_CANVAS:
			repaint_adjust_canvas(rc, con);
			break;
	}
}

void repaint_normal(repaint_context_t *rc, connections_t *con)
{

	mvwprintw(rc->topwin, 1, 1, "Press h for help...");

	/* Info window */
	wclear(rc->infowin);
	box(rc->infowin, 0, 0);
	char* info_title = " Information ";
	wattron(rc->infowin, A_BOLD);
	mvwprintw(rc->infowin, 0, rc->screen_width / 2 - strlen(info_title) / 2, info_title);
	wattroff(rc->infowin, A_BOLD);
	mvwprintw(rc->infowin, 1, 1, "Canvas w/h: %d/%d, size u/c: %d/%d, connections: %d",
			con->canvas_w, con->canvas_h,
			con->canvas_w * con->canvas_h * sizeof(int), con->comp_canvas_size,
			con->connections_n);
			
	/* The log window */
	wmove(rc->logwin, 0, 1);
	log_str_t *log = rc->head;
	int i = 0;
	int logwin_height;
	int logwin_width;
	(void) logwin_width; /* We don't use this one */
	getmaxyx(rc->logwin, logwin_height, logwin_width);

	/* Dump as many lines that fit into the window, from bottom up */
	while(log != NULL && i < logwin_height - 2)
	{
		mvwprintw(rc->logwin, logwin_height - (i + 2), 1, log->str);
		log = (log_str_t *) log->next;
		i++;
	}
	box(rc->logwin, 0, 0);
	char* log_title = " Log ";
	wattron(rc->logwin, A_BOLD);
	mvwprintw(rc->logwin, 0, rc->screen_width / 2 - strlen(log_title) / 2, log_title);
	wattroff(rc->logwin, A_BOLD);

	wrefresh(rc->topwin);
	wrefresh(rc->infowin);
	wrefresh(rc->logwin);
}


void repaint_help(repaint_context_t *rc)
{
	/* Top window */
	mvwprintw(rc->topwin, 1, 1, "Help: Press b to go back...");

	/* Help window */
	box(rc->helpwin, 0, 0);
	char* info_title = " Help ";
	wattron(rc->helpwin, A_BOLD);
	mvwprintw(rc->helpwin, 0, rc->screen_width / 2 - strlen(info_title) / 2, info_title);
	wattroff(rc->helpwin, A_BOLD);

#define NUMBER_OF_STRINGS 14
	char *strings[NUMBER_OF_STRINGS];
	strings[0] = "In the main window:";
	strings[1] = "Information shown about the canvas is as follows:";
	strings[2] = "First the width and height in pixels,";
	strings[3] = "then the size in bytes, uncompressed and compressed, respectivly";
	strings[4] = "Then the number of connections";
	strings[5] = "Press q to quit";
	strings[6] = "Press c to switch to the clients list window";
	strings[7] = "Press h to show this help window";
	strings[8] = "Press t to adjust the size of the canvas";
	strings[9] = "Press s to save the canvas";
	strings[10] = "";
	strings[11] = "In the Clients window:";
	strings[12] = "Use the arrow keys to navigate the list of connections";
	strings[13] = "Press spacebar to toggle write lock of selected client";
	for(int i = 0; i < NUMBER_OF_STRINGS; ++i)
	{
		if(i == 0 || i == 11)
			wattron(rc->helpwin, A_UNDERLINE);
		mvwprintw(rc->helpwin, i + 1, 1, strings[i]);
		if(i == 0 || i == 11)
			wattroff(rc->helpwin, A_UNDERLINE);
	}

	wrefresh(rc->topwin);
	wrefresh(rc->helpwin);
}

void repaint_clients(repaint_context_t *rc, connections_t *con)
{
	(void) con;
	/* Top window */
	mvwprintw(rc->topwin, 1, 1, "Clients: Press b to go back...");

	/* Client window */
	wclear(rc->clientwin);
	box(rc->clientwin, 0, 0);
	char* info_title = " Clients ";
	wattron(rc->clientwin, A_BOLD);
	mvwprintw(rc->clientwin, 0, rc->screen_width / 2 - strlen(info_title) / 2, info_title);
	wattroff(rc->clientwin, A_BOLD);

	/* Make sure the active connection exists! */
	if(rc->active_client >= (int) con->connections_n && rc->active_client != 0)
		rc->active_client = con->connections_n - 1;

	/* List clients */
	int i = 0;
	connection_t *tmp = con->connection;
	while(tmp != NULL)
	{
		if(i == rc->active_client)
			wattron(rc->clientwin, A_REVERSE);
		mvwprintw(rc->clientwin, i + 1, 1, "Client %d sent %d, received: %d. ",
				tmp->fd, tmp->sent_bytes, tmp->rec_bytes);
		if(tmp->write_lock != 0)
			wprintw(rc->clientwin, "Write protected!");
		if(i == rc->active_client)
			wattroff(rc->clientwin, A_REVERSE);
		tmp = (connection_t *) tmp->next;
		i++;
	}

	wrefresh(rc->topwin);
	wrefresh(rc->clientwin);
}

void repaint_adjust_canvas(repaint_context_t *rc, connections_t *con)
{
	/* Top window */
	mvwprintw(rc->topwin, 1, 1, "Adjust canvas: Press b to go back...");

	/* We're lazy and borrow the help window to draw our stuff */
	wclear(rc->helpwin);
	box(rc->helpwin, 0, 0);
	char* info_title = " Adjust canvas size ";
	wattron(rc->helpwin, A_BOLD);
	mvwprintw(rc->helpwin, 0, rc->screen_width / 2 - strlen(info_title) / 2, info_title);
	wattroff(rc->helpwin, A_BOLD);

	mvwprintw(rc->helpwin, 5, 5, "width: %d, height: %d", con->canvas_w_tmp, con->canvas_h_tmp);
	mvwprintw(rc->helpwin, 7, 5, "Press t to save or b to abort");

	wrefresh(rc->topwin);
	wrefresh(rc->helpwin);
}
