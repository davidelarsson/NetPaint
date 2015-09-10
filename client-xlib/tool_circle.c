#include "tool_circle.h"
#include "tool.h"
#include "network.h"
#include <math.h>

void tool_circle(tool_circle_context_t *context, canvas_event_t event)
{
	if(event.type == ButtonPress)
	{
		context->is_painting = True;
		context->start_x = event.x;
		context->start_y = event.y;
	}
	else if(event.type == ButtonRelease && context->is_painting == True)
	{
		context->is_painting = False;
		tool_circle_paint(context, event);
	}

	/* We do receive MotionNotify events as well, but since we have no need
	   for them, we just ignore them */

}

void tool_circle_paint(tool_circle_context_t *context, canvas_event_t event)
{
        /* We create a temporary image that is the result of the mouse movements */
        char *imageInitData = malloc(context->repaint_context->canvas->width *
                        context->repaint_context->canvas->height * sizeof(int) );
        int *p = (int *) imageInitData;
        for(int i = 0; i < context->repaint_context->canvas->width *
                       context->repaint_context->canvas->height; ++i)
                *p++ = 0x00000000; /* Make it all black at first */

        XImage *image = XCreateImage(context->repaint_context->display,
                context->repaint_context->visual, DEFAULT_DEPTH,
                ZPixmap, 0, imageInitData,
                context->repaint_context->canvas->width,
                context->repaint_context->canvas->height, 32, 0);

	int x = event.x - context->start_x;
	int y = event.y - context->start_y;
	if(x < 0)
		x = -x;
	if(y < 0)
		y = -y;
	unsigned length = sqrt(pow(x, 2) + pow(y, 2));

	/* Draw a white circle line on the temporary image */
	draw_circle_on_image(image, context->repaint_context->thickness, TMP_COLOR,
			context->start_x, context->start_y, length);

	process_image(context->repaint_context, image);

	XDestroyImage(image); /* This also free()s the data! */
}
