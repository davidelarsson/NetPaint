#ifndef TOOL_FREEHAND_H
#define TOOL_FREEHAND_H

#include "xlib.h"

/* A linked list used for storing mouse positions */
typedef struct ll_point_s {
	XPoint point;
	struct ll_point_t *next;
} ll_point_t;

typedef struct tool_freehand_context_s {
	repaint_context_t *repaint_context;
	ll_point_t *head;
	Bool is_painting;
} tool_freehand_context_t;

void ll_point_add(tool_freehand_context_t *context, XPoint p);
void ll_point_clear(tool_freehand_context_t *context);

void tool_freehand(tool_freehand_context_t *context, canvas_event_t event);
void tool_freehand_paint(tool_freehand_context_t *context);


#endif /* TOOL_FREEHAND_H */
