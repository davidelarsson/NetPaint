#include <X11/X.h>
#include "tool_freehand.h"
#include "tool.h"
#include "network.h"
#include <X11/Xlib.h>

void tool_freehand(tool_freehand_context_t *context, canvas_event_t event)
{
	if(event.type == ButtonPress)
	{
		context->is_painting = True;
		ll_point_clear(context);
		context->head->point.x = event.x;
		context->head->point.y = event.y;
	}
	else if(event.type == ButtonRelease && context->is_painting == True)
	{
		context->is_painting = False;
		tool_freehand_paint(context);
		ll_point_clear(context);
	}
	else if(event.type == MotionNotify && context->is_painting == True)
	{
		XPoint p;
		p.x = event.x;
		p.y = event.y;
		ll_point_add(context, p);
	}

}


void tool_freehand_paint(tool_freehand_context_t *context)
{
	/* We create a temporary image that is the result of the mouse movements */
	char *imageInitData = malloc(context->repaint_context->canvas->width *
			context->repaint_context->canvas->height * sizeof(int));
	int *p = (int *) imageInitData;
	for(int i = 0; i < context->repaint_context->canvas->width *
		       context->repaint_context->canvas->height; ++i)
		*p++ = 0x00000000; /* Make it all black at first */

	XImage *image = XCreateImage(context->repaint_context->display,
		context->repaint_context->visual,
		DEFAULT_DEPTH,
		ZPixmap, 0, imageInitData,
		context->repaint_context->canvas->width,
		context->repaint_context->canvas->height, 32, 0);
	
	ll_point_t *ptr = context->head;

	/* Paint white lines on the temporary image */
	while(ptr->next != NULL)
	{
		ll_point_t *next = (ll_point_t *) ptr->next;
		if(next->next == NULL)
			break;
		int start_x = ptr->point.x;
		int start_y = ptr->point.y;
		int end_x = next->point.x;
		int end_y = next->point.y;
		int thickness = context->repaint_context->thickness;
		draw_line_on_image(image, thickness, TMP_COLOR, start_x, start_y, end_x, end_y);
		ptr = (ll_point_t *) ptr->next;
	}

	process_image(context->repaint_context, image);

	XDestroyImage(image); /* This also free()s the data! */
}


/* Linked list functions */

void ll_point_add(tool_freehand_context_t *context, XPoint p)
{
	ll_point_t *tmp;
	tmp = context->head;
	while(tmp->next != NULL)
	{
		tmp = (ll_point_t *) tmp->next;
	}
	tmp->next = malloc(sizeof(ll_point_t));
	tmp->point = p;
	tmp = (ll_point_t *) tmp->next;
	tmp->next = NULL;
}


void ll_point_clear(tool_freehand_context_t *context)
{
	ll_point_t *tmp = context->head;
	context->head = malloc(sizeof(ll_point_t));
	context->head->next = NULL;
	while(tmp->next != NULL)
	{
		/* FIXME!!! Really? Really?!? Have I thought this through? */
		ll_point_t *tmp2 = tmp;
		tmp = (ll_point_t *) tmp->next;
		free(tmp2);
	}
}


