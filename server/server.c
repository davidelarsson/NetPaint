#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <zlib.h>
#include "server.h"
#include "network.h"
#include <ncurses.h>
#include "ncurses.h"

int screen_changed_signalled = 0;

int main(int argc, char* argv[])
{

        if(argc == 1)
        {
                do_server("127.0.0.1", 1234);
		return 0;

        } else if (argc == 3)
	{
		do_server(argv[1], atoi(argv[2]));
		return 0;
	}
	
	printf("Usage: %s IP PORT\n", argv[0]);
	return -1;

}


void handle_sigwinch(int sig)
{
	signal(SIGWINCH, SIG_IGN);
	(void) sig;
	screen_changed_signalled = 1;
	signal(SIGWINCH, handle_sigwinch);
}

int do_server(char* address, int port)
{

	/* Initialize the connections context */
	connections_t *connections = init_canvas(NULL);
	
	/* Init the UI */
	repaint_context_t *rc = init_repaint_context();

	/* create_server_socket() returns -1 if something went wrong */
	if((connections->server_sfd = create_server_socket(address, port)) < 0)
		return connections->server_sfd;

	char str[100];
	sprintf(str, "We've got a server socket: %d\n", connections->server_sfd);
	logstr(rc, str);

	signal(SIGWINCH, handle_sigwinch);

//	nodelay(stdscr, TRUE);
	event_loop(connections, rc);
	endwin();
	return 0;

}


int event_loop(connections_t *connections, repaint_context_t *rc)
{
	/* We need two file socket descriptor lists because select() modifies the fds list passed to it
	   each time it returns. */
	fd_set fds_list; /* Original list */
	fd_set fds_read; /* The list we let select() modify */
	FD_ZERO(&fds_list);
	FD_ZERO(&fds_read);
	FD_SET(connections->server_sfd, &fds_list);
	FD_SET(STDIN_FILENO, &fds_list);
	int close_down = 0;
	int fdmax = connections->server_sfd;

	while(close_down != 1)
	{
		repaint(rc, connections);
		fds_read = fds_list;
		int select_ret = select(fdmax + 1, &fds_read, NULL, NULL, NULL);
		if(errno == EINTR)
		{ /* Was a signal interrupting? */
			
		} else if(select_ret < 0)
		{
			printf("select() error: %s\n", strerror(errno));
			close(connections->server_sfd);
			exit(-1);
		}
		/* Check whether the terminal has been resized */
		if(screen_changed_signalled != 0)
		{
			screen_changed_signalled = 0;
			reinit_repaint_context(rc);
			getch(); /* Dummy read the KEY_RESIZE */
			char str[100];
			sprintf(str, "Window resized\n");
			logstr(rc, str);
			continue;
		}
		for(int fd = 0; fd <= fdmax; ++fd)
		{
			if (!FD_ISSET(fd, &fds_read))
				continue; /* Not this one, try the next one! */
			if(fd == STDIN_FILENO)
			{
				if(handle_ui(rc, connections) == -1)
					close_down = 1;
				repaint(rc, connections);
				continue;
			}
			if(fd == connections->server_sfd) /* Server socket. I.e. a new connection */
			{
				int new_fd = new_connection(connections);
				if(new_fd != -1)
				{
					FD_SET(new_fd, &fds_list);
					if(new_fd > fdmax)
						fdmax = new_fd;
					char str[100];
					sprintf(str, "New connection: %d\n", new_fd);
					logstr(rc, str);
					repaint(rc, connections);
				} else {
					char str[100];
					sprintf(str, "ERROR in new connection!\n");
					logstr(rc, str);
					exit(-1);
				}
			} else
				/* Handle client */
			{
				int res = handle_client(fd, connections);
				if(connections->err_list != NULL)
					log_errors(connections, rc);
				if(res == -1)
				{
					FD_CLR(fd, &fds_list); /* Client has closed connection */
					delete_connection(fd, connections);
					close(fd);
					char str[100];
					sprintf(str, "client %d has closed its connection\n", fd);
					logstr(rc, str);
					repaint(rc, connections);
				} else {
					update_canvas(connections);
					char str[100];
					sprintf(str, "client %d has updated its canvas\n", fd);
					logstr(rc, str);
					repaint(rc, connections);
				}
			}
		}
	}

	close(connections->server_sfd);
	return 0;

}

void log_errors(connections_t *con, repaint_context_t *rc)
{
	err_list_t *tmp = con->err_list;
	con->err_list = NULL;
	if(tmp == NULL)
		return;
	while(tmp != NULL)
	{
		logstr(rc, tmp->str);
	//	free(tmp->str);
		tmp = (err_list_t *) tmp->next;
	}
}

