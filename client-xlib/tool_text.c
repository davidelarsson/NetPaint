#include <X11/Xlib.h>
#include "tool_text.h"
#include "tool.h"
#include "network.h"

void tool_text(tool_text_context_t *context, canvas_event_t event)
{
	/* We do receive other events as well, but we are only interested in button presses */
	if(event.type == ButtonPress)
	{
		tool_text_paint(context, event);
	}

}


void tool_text_paint(tool_text_context_t *context, canvas_event_t event)
{
	/* Create a pixmap on the server on which we draw the text */
	Pixmap pixmap = XCreatePixmap(context->repaint_context->display,
			context->repaint_context->window,
			context->repaint_context->canvas->width,
			context->repaint_context->canvas->height,
			context->repaint_context->canvas->depth);
	XFillRectangle(context->repaint_context->display, pixmap,
			DefaultGC(context->repaint_context->display,
			context->repaint_context->screen), 0, 0,
			context->repaint_context->canvas->width,
			context->repaint_context->canvas->height);
	XGCValues values;
	values.foreground = TMP_COLOR;
	GC gc = XCreateGC(context->repaint_context->display,
			context->repaint_context->window, GCForeground, &values);
	XDrawString(context->repaint_context->display, pixmap, gc, event.x, event.y,
		       context->repaint_context->input_string,
		       strlen(context->repaint_context->input_string));

	/* Now we create an image from the pixmap. */
	XImage *image = XGetImage(context->repaint_context->display, pixmap, 0, 0,
			context->repaint_context->canvas->width,
			context->repaint_context->canvas->height,
			TMP_COLOR, /* Plane mask. Only copy the pixels set to the right color */
			ZPixmap);
	process_image(context->repaint_context, image);
	XFreePixmap(context->repaint_context->display, pixmap);
	XDestroyImage(image);
}

