#ifndef NETWORK_H
#define NETWORK_H

#include "netpaint.h"
#include <X11/Xlib.h>
#include "xlib.h"

int init_network(char* address, int port);
int ready_read(int sfd, repaint_context_t *repaint_context);
void send_image(XImage *image, repaint_context_t *repaint_context);
int get_all(int sfd, char *buffer, int *len);
int send_all(int sfd, char *buffer, int *len);

#endif /* NETWORK_H */

