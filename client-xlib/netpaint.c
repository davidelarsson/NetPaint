#include "netpaint.h"
#include <stdlib.h>
#include "xlib.h"
#include "network.h"
#include <sys/select.h>
#include <X11/Xlib.h>



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


/* I would very much like to have this one in a header file! */
int handle_xevent(repaint_context_t *context, tool_contexts_t *tool_contexts);


int event_loop(int sfd, repaint_context_t *repaint_context, tool_contexts_t *tool_contexts)
{

        /* select kind of events we are interested in */
        XSelectInput(repaint_context->display, repaint_context->window,
		(ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask |
		 PointerMotionMask | StructureNotifyMask));

        XMapWindow(repaint_context->display, repaint_context->window);
	int win_fd = ConnectionNumber(repaint_context->display);

	fd_set fds_list;
	fd_set fds_tmp;
	FD_ZERO(&fds_list);
	FD_SET(sfd, &fds_list);
	FD_SET(win_fd, &fds_list);

	int fdmax = FD_SETSIZE;

	/* FIXME!!! Why do I need to call this before the loop?
	   Is it because the XMapWindow() call? But why isn't that
	   just put into the queue?
	 */
	handle_xevent(repaint_context, tool_contexts);

	int break_flag = 0;
	while(break_flag != 1)
	{
		fds_tmp = fds_list;
		if(select(fdmax + 1, &fds_tmp, NULL, NULL, NULL) == -1)
		{
			printf("select() error: %s\n", strerror(errno));
			close(sfd);
			return -1;
		}
		if(FD_ISSET(win_fd, &fds_tmp))
		{
 			if(handle_xevent(repaint_context, tool_contexts) == 1)
			{
				printf("Quitting...\n");
				break_flag = 1;
			}
		}
		if(FD_ISSET(sfd, &fds_tmp))
		{
			printf("Network event received!\n");
			if(ready_read(sfd, repaint_context) != 0)
			{
				printf("Error received from server!\n");
				close(sfd);
				return -1;
			}
			/* FIXME!!! Uh-gly! Do I really need this?! */
			repaint(repaint_context);
			handle_xevent(repaint_context, tool_contexts);
		}
	}
	return 0;
}

int init_netpaint(char* ip, char* port)
{

	repaint_context_t *repaint_context = init_repaint_context();
	int sfd;
	if((sfd = init_network(ip, atoi(port))) < 0)
	{
		printf("ERROR! Could not initialize network!\n");
		return -1;
	}
	if(ready_read(sfd, repaint_context) < 0)
	{
		printf("ERROR! Could not get the first canvas!\n");
		return -1;
	}
	repaint_context->canvas = repaint_context->canvases[0];
	repaint_context->sfd = sfd;
		
	tool_contexts_t *tool_contexts = init_tool_contexts(repaint_context);

	event_loop(sfd, repaint_context, tool_contexts);

	XCloseDisplay(repaint_context->display);
	(void) ip;
	(void) port;
	return 0;
}

int main(int argc, char* argv[])
{
	if(argc != 3) 
	{

		/* FIXME!!! This is not to be kept! */
		init_netpaint("127.0.0.1", "1234");
		exit(0);
//		printf("Usage: %s IP PORT\n", argv[0]);
//		exit(1);
		
	}

	init_netpaint(argv[1], argv[2]);

	return 0;
}

