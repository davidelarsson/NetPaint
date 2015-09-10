#include "tool_line.h"
#include "tool.h"
#include "network.h"

void tool_line(tool_line_context_t *context, canvas_event_t event)
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
		tool_line_paint(context, event);
	}

	/* We do receive MotionNotify events as well, but since we have no need
	   for them, we just ignore them */

}

void tool_line_paint(tool_line_context_t *context, canvas_event_t event)
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

	/* Draw a line on the temporary image */
	draw_line_on_image(image, context->repaint_context->thickness, TMP_COLOR,
			context->start_x, context->start_y, event.x, event.y);

	process_image(context->repaint_context, image);
/*
	if(context->repaint_context->active_canvas == 0)
	{
 	       for(int i = 0; i < context->repaint_context->canvas->width; ++i)
        	        for(int j = 0; j < context->repaint_context->canvas->height; ++j)
				if(((int) XGetPixel(image, i, j)) == tmp_color)
               	                	XPutPixel(image, i, j, real_color);
		send_image(image, context->repaint_context);
	}
	else
	{
 	       for(int i = 0; i < context->repaint_context->canvas->width; ++i)
        	        for(int j = 0; j < context->repaint_context->canvas->height; ++j)
				if(((int) XGetPixel(image, i, j)) == tmp_color)
               	                	XPutPixel(context->repaint_context->canvas, i, j, real_color);
	}
*/


	XDestroyImage(image); /* This also free()s the data! */
}
