#ifndef TOOL_TEXT_H
#define TOOL_TEXT_H

#include "xlib.h"

typedef struct tool_text_context_s {
	repaint_context_t *repaint_context;
//	char *string;
} tool_text_context_t;

void tool_text(tool_text_context_t *context, canvas_event_t event);
void tool_text_paint(tool_text_context_t *context, canvas_event_t event);

#endif /* TOOL_TEXT_H */
