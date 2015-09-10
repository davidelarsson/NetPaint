#ifndef TOOL_RECTANGLE_H
#define TOOL_RECTANGLE_H

#include "xlib.h"

typedef struct tool_rectangle_context_s {
	Bool is_painting;
	unsigned start_x;
	unsigned start_y;
	repaint_context_t *repaint_context;
} tool_rectangle_context_t;

void tool_rectangle(tool_rectangle_context_t *context, canvas_event_t event);
void tool_rectangle_paint(tool_rectangle_context_t *context, canvas_event_t event);

#endif /* RECTANGLE_H */
