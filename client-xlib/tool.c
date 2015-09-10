#include <X11/X.h>
#include "tool.h"
#include "network.h"
#include <X11/Xlib.h>

void process_image(repaint_context_t *repaint_context, XImage *image)
{
	int real_color = (0xFF000000 | repaint_context->colors[repaint_context->active_color]);

        /* If we're painting on canvases[0] (or logically canvas 1), adjust the color and
           send it to the server */
        if(repaint_context->active_canvas == 0)
        {
               for(int i = 0; i < repaint_context->canvas->width; ++i)
                        for(int j = 0; j < repaint_context->canvas->height; ++j)
                                if(((int) XGetPixel(image, i, j)) == TMP_COLOR)
                                        XPutPixel(image, i, j, real_color);
                send_image(image, repaint_context);
        } 
        /* If we're painting locally, just copy the image to the active canvas */
        else
        {
               for(int i = 0; i < repaint_context->canvas->width; ++i)
                        for(int j = 0; j < repaint_context->canvas->height; ++j)
                                if(((int) XGetPixel(image, i, j)) == TMP_COLOR)
                                        XPutPixel(repaint_context->canvas, i, j, real_color);
        }
}

void draw_circle_on_image(image, thickness, color, start_x, start_y, radius)
	XImage *image;
	unsigned thickness;
	int color;
	int start_x;
	int start_y;
	unsigned radius;
{
  int cx = start_x;
  int cy = start_y;
  int error = -radius;
  int x = radius;
  int y = 0;
 
  // The following while loop may be altered to 'while (x > y)' for a
  // performance benefit, as long as a call to 'plot4points' follows
  // the body of the loop. This allows for the elimination of the
  // '(x != y)' test in 'plot8points', providing a further benefit.
  //
  // For the sake of clarity, this is not shown here.
  while (x >= y)
  {
    plot8points(image, thickness, color, cx, cy, x, y);
 
    error += y;
    ++y;
    error += y;
 
    // The following test may be implemented in assembly language in
    // most machines by testing the carry flag after adding 'y' to
    // the value of 'error' in the previous step, since 'error'
    // nominally has a negative value.
    if (error >= 0)
    {
      error -= x;
      --x;
      error -= x;
    }
  }
}
 
void plot8points(XImage *image, unsigned thickness, int color, int cx, int cy, int x, int y)
{
  plot4points(image, thickness, color, cx, cy, x, y);
  if (x != y) plot4points(image, thickness, color, cx, cy, y, x);
}
 
// The '(x != 0 && y != 0)' test in the last line of this function
// may be omitted for a performance benefit if the radius of the
// circle is known to be non-zero.
void plot4points(XImage *image, unsigned thickness, int color, int cx, int cy, int x, int y)
{
//  setPixel(cx + x, cy + y);
  draw_point_on_image(image, thickness, color, cx + x, cy + y);

//  if (x != 0) setPixel(cx - x, cy + y);
  if(x !=0)
	  draw_point_on_image(image, thickness, color, cx - x, cy + y);

//  if (y != 0) setPixel(cx + x, cy - y);
  if(y !=0)
	  draw_point_on_image(image, thickness, color, cx + x, cy - y);

//  if (x != 0 && y != 0) setPixel(cx - x, cy - y);
  if(x !=0 && y != 0)
	  draw_point_on_image(image, thickness, color, cx - x, cy - y);
}

void draw_line_on_image(image, thickness, color, start_x, start_y, end_x, end_y)
	XImage *image;
	unsigned thickness;
	int color;
	int start_x;
	int start_y;
	int end_x;
	int end_y;
{
	/* Oh, and it's all stolen code in this function */
	int x2 = end_x;
	int x1 = start_x;
	int y2 = end_y;
	int y1 = start_y;

	int deltax = abs(x2 - x1);		// The difference between the x's
	int deltay = abs(y2 - y1);		// The difference between the y's
	int x = x1;			   	// Start x off at the first pixel
	int y = y1;			   	// Start y off at the first pixel
	int xinc1;
	int xinc2;
	int yinc1;
	int yinc2;
	if (x2 >= x1)			 	// The x-values are increasing
	{
	  xinc1 = 1;
	  xinc2 = 1;
	}
	else						  // The x-values are decreasing
	{
	  xinc1 = -1;
	  xinc2 = -1;
	}
 
	if (y2 >= y1)			 	// The y-values are increasing
	{
	  yinc1 = 1;
	  yinc2 = 1;
	}
	else						  // The y-values are decreasing
	{
	  yinc1 = -1;
	  yinc2 = -1;
	}
	int den;
	int num;
	int numadd;
	int numpixels;
 
	if (deltax >= deltay)	 	// There is at least one x-value for every y-value
	{
	  xinc1 = 0;				  // Don't change the x when numerator >= denominator
	  yinc2 = 0;				  // Don't change the y for every iteration
	  den = deltax;
	  num = deltax / 2;
	  numadd = deltay;
	  numpixels = deltax;	 	// There are more x-values than y-values
	}
	else						  // There is at least one y-value for every x-value
	{
	  xinc2 = 0;				  // Don't change the x for every iteration
	  yinc1 = 0;				  // Don't change the y when numerator >= denominator
	  den = deltay;
	  num = deltay / 2;
	  numadd = deltax;
	  numpixels = deltay;	 	// There are more y-values than x-values
	}
 
	int curpixel;
	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
	  draw_point_on_image(image, thickness, color, x, y);		 	// Draw the current pixel
	  num += numadd;			  // Increase the numerator by the top of the fraction
	  if (num >= den)		 	// Check if numerator >= denominator
	  {
		num -= den;		   	// Calculate the new numerator value
		x += xinc1;		   	// Change the x as appropriate
		y += yinc1;		   	// Change the y as appropriate
	  }
	  x += xinc2;			 	// Change the x as appropriate
	  y += yinc2;			 	// Change the y as appropriate
	}
}


void draw_point_on_image(XImage *image, unsigned thickness, int color, int x, int y)
{
	thickness *= 2;
	x = x - thickness / 2;
	y = y - thickness / 2;
	for(int i = x; i < x + (int) thickness / 2; ++i)
		for(int j = y; j < y + (int) thickness / 2; ++j)
			draw_pixel_on_image(image, color, i, j);
			
}

/* XPutPixel is not safe to call if we're outside of our image
   That's why we wrap it in our own function */
void draw_pixel_on_image(XImage *image, int color, int x, int y)
{
	if(x < 0 || x >= image->width || y < 0 || y >= image->height)
		return;
	else
		XPutPixel(image, x, y, color);

}



