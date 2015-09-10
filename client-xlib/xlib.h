#ifndef XLIB_H
#define XLIB_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANVAS_DEFAULT_WIDTH 800
#define CANVAS_DEFAULT_HEIGHT 400
#define CANVAS_FRAME_WIDTH 5
#define SCROLL_STEPS 10
#define WINDOW_DEFAULT_WIDTH 800
#define WINDOW_DEFAULT_HEIGHT 600
#define BUTTON_FRAME_DEFAULT_HEIGHT 30
#define COLOR_FRAME_DEFAULT_HEIGHT 50
#define NUMBER_OF_COLORS 20 /* This must be at least 8 */
#define COLOR_ROWS 2 /* This should be set to 1 or an even number */
#define NUMBER_OF_TOOLS 5 /* This never really changes... */
#define STATUS_FRAME_DEFAULT_HEIGHT 30
#define DEFAULT_DEPTH 24
#define MAX_CANVASES 20

/* The order of the tools is important for repaint_normal() in xlib.c */
typedef enum active_tool_s { FREEHAND, LINE, RECTANGLE, CIRCLE, TEXT } active_tool_t;
typedef enum active_window_s { NORMAL, HELP, COLOR, TEXT_INPUT } active_window_t;

typedef struct image_s {
	int width;
	int height;
	int *data;
} image_t;

typedef struct canvas_event_s {
	int type;
	int x;
	int y;
} canvas_event_t;

typedef struct repaint_context_s {
	Display *display;
	Window window;
	Visual *visual;
	int sfd;
	int screen;
	unsigned window_width;
	unsigned window_height;
	XImage *canvas;
	int scroll_x;
	int scroll_y;
	unsigned number_of_canvases;
	unsigned active_canvas;
	XImage **canvases;
	active_tool_t active_tool;
	active_window_t active_window;
	int active_color;
	int *colors;
	unsigned thickness;
	char *input_string;
} repaint_context_t;

void repaint(repaint_context_t *context);
void handle_event_helpscreen(repaint_context_t *context, XEvent event);
void handle_event_text_input(repaint_context_t *context, XEvent event);
void handle_event_text(repaint_context_t *context, XEvent event);
repaint_context_t *init_repaint_context();
void init_tools(repaint_context_t *repaint_context);
void create_canvas(repaint_context_t *context, int width, int height);
void delete_canvas(repaint_context_t *context);
void scroll_left(repaint_context_t *context);
void scroll_right(repaint_context_t *context);
void scroll_up(repaint_context_t *context);
void scroll_down(repaint_context_t *context);
//void handle_event_normal(repaint_context_t *context, XEvent event);

#endif /* XLIB_H */

