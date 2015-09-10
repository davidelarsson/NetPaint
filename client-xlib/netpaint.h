#ifndef NETPAINT_H
#define NETPAINT_H

#include "network.h"
#include "xlib.h"
#include "tool_freehand.h"
#include "tool_line.h"
#include "tool_rectangle.h"
#include "tool_circle.h"
#include "tool_text.h"
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]);
int init_netpaint();

typedef struct tool_contexts_s {
	tool_line_context_t *tool_line_context;
	tool_rectangle_context_t *tool_rectangle_context;
	tool_freehand_context_t *tool_freehand_context;
	tool_circle_context_t *tool_circle_context;
	tool_text_context_t *tool_text_context;
} tool_contexts_t;

void handle_event_normal(repaint_context_t *repaint_context, tool_contexts_t *tool_contexts, XEvent event);
void xevent_loop(repaint_context_t *repaint_context, tool_contexts_t *tool_contexts);
repaint_context_t *init_repaint_context();
tool_contexts_t *init_tool_contexts(repaint_context_t *repaint_context);
void handle_event_helpscreen(repaint_context_t *context, XEvent event);
void handle_event_colorscreen(repaint_context_t *repaint_context, XEvent event);
int event_loop(int sfd, repaint_context_t *repaint_context, tool_contexts_t *tool_contexts);


#endif /* NETPAINT_H */

