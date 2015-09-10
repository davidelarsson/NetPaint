#ifndef TOOL_CIRCLE_H
#define TOOL_CIRCLE_H

#include "xlib.h"

typedef struct tool_circle_context_s {
	Bool is_painting;
	unsigned start_x;
	unsigned start_y;
	repaint_context_t *repaint_context;
} tool_circle_context_t;

void tool_circle(tool_circle_context_t *context, canvas_event_t event);
void tool_circle_paint(tool_circle_context_t *context, canvas_event_t event);

#endif /* TOOL_CIRCLE_H */
