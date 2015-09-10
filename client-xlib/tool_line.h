#ifndef TOOL_LINE_H
#define TOOL_LINE_H

#include "xlib.h"

typedef struct tool_line_context_s {
	Bool is_painting;
	unsigned start_x;
	unsigned start_y;
	repaint_context_t *repaint_context;
} tool_line_context_t;

void tool_line(tool_line_context_t *context, canvas_event_t event);
void tool_line_paint(tool_line_context_t *context, canvas_event_t event);

#endif /* TOOL_LINE_H */
