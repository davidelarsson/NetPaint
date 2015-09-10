#ifndef TOOL_H
#define TOOL_H

#include "xlib.h"

#define TMP_COLOR 0x00000001


void process_image(repaint_context_t *context, XImage *image);
void draw_line_on_image(XImage *image, unsigned thickness, int color, int start_x, int start_y, int end_x, int end_y);
void draw_point_on_image(XImage *image, unsigned thickness, int color, int x, int y);
void draw_pixel_on_image(XImage *image, int color, int x, int y);
void draw_circle_on_image(XImage *image, unsigned thickness, int color, int start_x, int start_y, unsigned radius);
void plot8points(XImage *image, unsigned thickness, int color, int cx, int cy, int x, int y);
void plot4points(XImage *image, unsigned thickness, int color, int cx, int cy, int x, int y);

#endif /* TOOL_H */
