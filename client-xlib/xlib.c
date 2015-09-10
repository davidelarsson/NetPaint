#include "xlib.h"
#include "tool.h"
#include "tool_line.h"
#include "tool_rectangle.h"
#include "tool_freehand.h"
#include "tool_circle.h"
#include "tool_text.h"
#include "netpaint.h"
#include "file.h"

int handle_xevent(repaint_context_t *context, tool_contexts_t *tool_contexts);

void repaint_colorscreen(repaint_context_t *context)
{

	int window_x, window_y;
        Window root_window;
        unsigned int window_border_width;
        unsigned int window_depth;
        XGetGeometry(context->display, context->window, &root_window,
                        &window_x, &window_y, &context->window_width, &context->window_height,
                        &window_border_width, &window_depth);

        /* We buffer the screen in a pixmap before copying it onto screen */
        Pixmap pixmap = XCreatePixmap(context->display, context->window,
                        context->window_width, context->window_height, DEFAULT_DEPTH);

        /* Make it black */
        XGCValues values;
        values.foreground = 0x00000000;
        GC gc = XCreateGC(context->display, pixmap, GCForeground, &values);
        XFillRectangle(context->display, pixmap, gc, 0, 0,
                        context->window_width, context->window_height);

        /* Green text */
        values.foreground = 0x0000FF00;
        gc = XCreateGC(context->display, pixmap, GCForeground, &values);
        char* str_line0 = "Adjust color";
        char* str_line1 = "a increases and z decreases the red";
        char* str_line2 = "s increases and x decreases the green";
        char* str_line3 = "d increases and c decreases the blue";
        char str_line4[100];
        sprintf(str_line4, "Current color: 0x%06X", context->colors[context->active_color]);
        char* str_end = "Press q when done...";
        int window_height = context->window_height;
        XDrawString(context->display, pixmap, gc, 5, 15, str_line0, strlen(str_line0));
        XDrawString(context->display, pixmap, gc, 5, 30, str_line1, strlen(str_line1));
        XDrawString(context->display, pixmap, gc, 5, 45, str_line2, strlen(str_line2));
        XDrawString(context->display, pixmap, gc, 5, 60, str_line3, strlen(str_line3));
        XDrawString(context->display, pixmap, gc, 5, 75, str_line4, strlen(str_line4));
        XDrawString(context->display, pixmap, gc, 50, window_height - 20, str_end, strlen(str_end));

        /* We paint square representing the current color */
        int x1 = 100;
        int y1 = 100;
        int x2 = 50;
        int y2 = 50;
        values.foreground = context->colors[context->active_color];
        gc = XCreateGC(context->display, pixmap, GCForeground, &values);
        XFillRectangle(context->display, pixmap, gc, x1, y1, x2, y2);

        /* ...surrounded by a white rectangle */
        values.foreground = 0x00FFFFFF;
        values.line_width = 5;
        gc = XCreateGC(context->display, pixmap, (GCLineWidth | GCForeground), &values);
        XDrawRectangle(context->display, pixmap, gc, x1, y1, x2, y2);

	/* Copy the pixmap onto screen */
        XCopyArea(context->display, pixmap, context->window,
                        DefaultGC(context->display, context->screen), 0, 0,
                        context->window_width, context->window_height,0, 0);
        XFreePixmap(context->display, pixmap);

}


void repaint_helpscreen(repaint_context_t *context)
{
	/* We buffer the screen in a pixmap before copying it onto screen */
	Pixmap pixmap = XCreatePixmap(context->display, context->window,
			context->window_width, context->window_height, DEFAULT_DEPTH);

	/* Make it black */
	XGCValues values;
	values.foreground = 0x00000000;
        GC gc = XCreateGC(context->display, pixmap, GCForeground, &values);
	XFillRectangle(context->display, pixmap, gc, 0, 0,
			context->window_width, context->window_height);

	/* Green text */
	values.foreground = 0x0000FF00;
        gc = XCreateGC(context->display, pixmap, GCForeground, &values);
#define NUMBER_OF_STRINGS 24
	char* strings[NUMBER_OF_STRINGS];
	strings[0] = "This is the Xlib version of NetPaint - a graphics editor with network support";
	strings[1] = "";
	strings[2] = "You have up to 20 canvases, where canvas 1 is the one on the server,";
	strings[3] = "and the rest are local on your client";
	strings[4] = "";
	strings[5] = "Use the tools above and the colors below. Right-click on a color to adjust it";
	strings[6] = "";
	strings[7] = "Keyboard commands are as follows:";
	strings[8] = "";
	strings[9] = "q = quit";
	strings[10] = "a = increase thickness";
	strings[11] = "z = decrease thickness";
	strings[12] = "c = copy current canvas into a new local one";
	strings[13] = "n = create a new canvas with black background";
	strings[14] = "d = delete local canvas";
	strings[15] = "s = save canvas";
	strings[16] = "1 - 0 = switch to canvas n";
	strings[17] = "PgUp = next canvas";
	strings[18] = "PgDown = previous canvas";
	strings[19] = "arrow keys = scroll canvas";
	strings[20] = "";
	strings[21] = "";
	strings[22] = "";
	strings[23] = "";
	for(int i = 0; i < NUMBER_OF_STRINGS; ++i)
		XDrawString(context->display, pixmap, gc, 5, (i + 1) * 15,
				strings[i], strlen(strings[i]));

	char* str_end = "Press any key to continue...";
	XDrawString(context->display, pixmap, gc, 50, context->window_height - 20,
		       str_end, strlen(str_end));
	
	/* Copy the pixmap onto screen */
	XCopyArea(context->display, pixmap, context->window,
			DefaultGC(context->display, context->screen), 0, 0,
			context->window_width, context->window_height,0, 0);
}

void repaint_text_input(repaint_context_t *context)
{
	/* We buffer the screen in a pixmap before copying it onto screen */
	Pixmap pixmap = XCreatePixmap(context->display, context->window,
			context->window_width, context->window_height, DEFAULT_DEPTH);

	/* Make it black */
	XGCValues values;
	values.foreground = 0x00000000;
        GC gc = XCreateGC(context->display, pixmap, GCForeground, &values);
	XFillRectangle(context->display, pixmap, gc, 0, 0,
			context->window_width, context->window_height);

	/* Green text */
	values.foreground = 0x0000FF00;
        gc = XCreateGC(context->display, pixmap, GCForeground, &values);
	char *info_str = "Enter a text string:";
	XDrawString(context->display, pixmap, gc, 20, 20, info_str, strlen(info_str));
	XDrawString(context->display, pixmap, gc, 20, 40,
			context->input_string, strlen(context->input_string));

	char* str_end = "Press any key to continue...";
	XDrawString(context->display, pixmap, gc, 50, context->window_height - 20,
		       str_end, strlen(str_end));
	
	/* Copy the pixmap onto screen */
	XCopyArea(context->display, pixmap, context->window,
			DefaultGC(context->display, context->screen), 0, 0,
			context->window_width, context->window_height,0, 0);
}

void repaint_normal(repaint_context_t *context)
{
	/* First, fix the scrolling. Window size might have been changed! */
	/* Check whether the x coordinate is valid */
	/* scroll_x must not be larger than max_width */
	/* FIXME!!! This is just taken from below, I dunno how it really works... */
	int max_width = context->window_width - 2 * CANVAS_FRAME_WIDTH;
	if(context->canvas->width < max_width);
	else if((int) (context->scroll_x + SCROLL_STEPS + max_width) > context->canvas->width)
		context->scroll_x = context->canvas->width - max_width;

	int max_height = context->window_height -
		(2 * CANVAS_FRAME_WIDTH + BUTTON_FRAME_DEFAULT_HEIGHT);
	if(context->canvas->height < max_height);
	else if((int) (context->scroll_y + SCROLL_STEPS + max_height) > context->canvas->height)
		context->scroll_y = context->canvas->height - max_height;

	/* We buffer the screen in a pixmap before copying it onto screen */
	Pixmap pixmap = XCreatePixmap(context->display, context->window,
			context->window_width, context->window_height, DEFAULT_DEPTH);

	/* We first declare our values */
	XGCValues values;
	values.foreground = 0x00000000;
        GC gc = XCreateGC(context->display, pixmap, GCForeground, &values);

	XFillRectangle(context->display, pixmap, gc, 0, 0,
			context->window_width, context->window_height);

	values.foreground = 0x0000FF00;
        gc = XCreateGC(context->display, pixmap, GCForeground, &values);

	/* Draw the button frame */
	XDrawRectangle(context->display, pixmap, gc,
			0, 0, context->window_width - 1, BUTTON_FRAME_DEFAULT_HEIGHT);

	char *str_tools[NUMBER_OF_TOOLS];
	for(int i = 0; i < NUMBER_OF_TOOLS; ++i)
		str_tools[i] = NULL;
	str_tools[0] = "Freehand";
	str_tools[1] = "Line";
	str_tools[2] = "Rectangle";
	str_tools[3] = "Circle";
	str_tools[4] = "Text";

	char *str_active_tool = "*";

	for(unsigned i = 0; i < NUMBER_OF_TOOLS; ++i)
	{
		XDrawLine(context->display, pixmap,
				gc,
				i * (context->window_width - 1) / NUMBER_OF_TOOLS, 0,
				i * (context->window_width - 1) / NUMBER_OF_TOOLS,
				BUTTON_FRAME_DEFAULT_HEIGHT);

		XDrawString(context->display, pixmap,
				gc,
				i * (context->window_width - 1) / NUMBER_OF_TOOLS + 20,15,
				str_tools[i], strlen(str_tools[i]));
		if(i == context->active_tool)
			XDrawString(context->display, pixmap, gc,
					i * (context->window_width - 1) / NUMBER_OF_TOOLS + 5,15,
					str_active_tool, strlen(str_active_tool));

	}

	/* Draw a white square on which we paint our canvas */
	values.foreground = 0x00FFFFFF;
        gc = XCreateGC(context->display, pixmap, GCForeground, &values);
	XFillRectangle(context->display, pixmap, gc,
			0, BUTTON_FRAME_DEFAULT_HEIGHT + 1,
			context->window_width, context->window_height -
			(BUTTON_FRAME_DEFAULT_HEIGHT + COLOR_FRAME_DEFAULT_HEIGHT +
			STATUS_FRAME_DEFAULT_HEIGHT));

	/* Draw the canvas */
	int width_to_copy;
	int height_to_copy;

	if(context->canvas->width > ((int) context->window_width - 2 * CANVAS_FRAME_WIDTH))
		width_to_copy = context->window_width - 2 * CANVAS_FRAME_WIDTH;
	else
		width_to_copy = context->canvas->width;

	if(context->canvas->height > (int) context->window_height - 
			(BUTTON_FRAME_DEFAULT_HEIGHT + COLOR_FRAME_DEFAULT_HEIGHT +
			STATUS_FRAME_DEFAULT_HEIGHT + 2 * CANVAS_FRAME_WIDTH))
		height_to_copy = context->window_height - (BUTTON_FRAME_DEFAULT_HEIGHT +
				COLOR_FRAME_DEFAULT_HEIGHT + STATUS_FRAME_DEFAULT_HEIGHT +
			      	2 * CANVAS_FRAME_WIDTH);
	else
		height_to_copy = context->canvas->height;

	XPutImage(context->display, pixmap,
			DefaultGC(context->display, context->screen), context->canvas,
			/* Source coordinates */
			context->scroll_x, context->scroll_y,
			/* Destination coordinates */
		       	CANVAS_FRAME_WIDTH, BUTTON_FRAME_DEFAULT_HEIGHT + CANVAS_FRAME_WIDTH,
			/* Width and height to copy */
			width_to_copy, height_to_copy);

	/* Draw the color boxes */
	int y_offset = context->window_height -
		(COLOR_FRAME_DEFAULT_HEIGHT + STATUS_FRAME_DEFAULT_HEIGHT);

	for(int i = 0; i < NUMBER_OF_COLORS; ++i) {
		XGCValues values;
		int x1 = (context->window_width / NUMBER_OF_COLORS * COLOR_ROWS) * (i / COLOR_ROWS);
		int y1 = y_offset + (COLOR_FRAME_DEFAULT_HEIGHT / COLOR_ROWS) * (i % COLOR_ROWS);
		int x2 = context->window_width / NUMBER_OF_COLORS * COLOR_ROWS;
		int y2 = COLOR_FRAME_DEFAULT_HEIGHT / COLOR_ROWS;
		values.foreground = context->colors[i];
		GC gc = XCreateGC(context->display, pixmap, GCForeground, &values);
		XFillRectangle(context->display, pixmap, gc, x1, y1, x2, y2);

		/* If this is the active color, mark it! */
		if(i == context->active_color)
		{
			values.foreground = ~context->colors[context->active_color];
			gc = XCreateGC(context->display, pixmap, GCForeground, &values);
			char* str_active_color = "*";
			XDrawString(context->display, pixmap, gc, x1 + 10, y1 + 15,
			str_active_color, strlen(str_active_color));

		}
	}

	/* Draw the status line */
	/* Note that the displayed number of the active canvas is incremented by one.
	   It looks better that way! */
	char* str_status = malloc(100);
	sprintf(str_status, "Thickness: %2d   Canvas: %2d/%2d      scroll x: %4d      scroll y: %4d     Press h for help",
		       context->thickness, context->active_canvas + 1, context->number_of_canvases,
		       context->scroll_x, context->scroll_y);

	values.foreground = 0x0000FF00;
        gc = XCreateGC(context->display, pixmap, GCForeground, &values);
	XDrawString(context->display, pixmap, gc, 0,
			context->window_height - STATUS_FRAME_DEFAULT_HEIGHT + 15,
			str_status, strlen(str_status));
	free(str_status);

	/* Copy the pixmap onto screen */
	XCopyArea(context->display, pixmap, context->window,
			DefaultGC(context->display, context->screen), 0, 0,
			context->window_width, context->window_height,0, 0);
	XFreePixmap(context->display, pixmap);
}

void repaint(repaint_context_t *context)
{

	/* This is information that might be needed by other functions later.
	   If so, save the information in the context struct.
	   But for now, we just need a place to dump it.
	 */
	int window_x, window_y;
	Window root_window;
	unsigned int window_border_width;
	unsigned int window_depth;

	/* FIXME!!! I don't need this if I catch ConfigureNotify events, do I? */
	if (XGetGeometry(context->display, context->window, &root_window,
			&window_x, &window_y, &context->window_width, &context->window_height,
			&window_border_width, &window_depth) == False)
	{
		printf("ERROR: repaint() failed! Could not get window geometry.\n");
	}
	
	switch(context->active_window) {
		case NORMAL:
			repaint_normal(context);
			break;
		case HELP:
			repaint_helpscreen(context);
			break;
		case COLOR:
			repaint_colorscreen(context);
			break;
		case TEXT_INPUT:
			repaint_text_input(context);
			break;
		default:
			printf("No active window?!\n");
			break;
	}
	
}

repaint_context_t *init_repaint_context()
{
	repaint_context_t *repaint_context = malloc(sizeof(repaint_context_t));
	repaint_context->window_width = WINDOW_DEFAULT_WIDTH;
	repaint_context->window_height = WINDOW_DEFAULT_HEIGHT;

	/* Connect to the X server */
	/* NULL defaults to the DISPLAY environment variable */
	repaint_context->display = XOpenDisplay(NULL); 
	if (repaint_context->display == NULL)
	{
		fprintf(stderr, "Cannot open display\n");
		exit(1);
	}

	repaint_context->screen = DefaultScreen(repaint_context->display);
	repaint_context->active_tool = FREEHAND;
	repaint_context->active_window = NORMAL;
	repaint_context->thickness = 1;
	repaint_context->scroll_x = 0;
	repaint_context->scroll_y = 0;
	char *tmp_str = "text";
	repaint_context->input_string = malloc(sizeof(char) * strlen(tmp_str));
	strcpy(repaint_context->input_string, tmp_str);

	repaint_context->window = XCreateSimpleWindow(repaint_context->display,
		        DefaultRootWindow(repaint_context->display), 0, 0,
		       	repaint_context->window_width, repaint_context->window_height, 1,
			WhitePixel(repaint_context->display, repaint_context->screen),
			BlackPixel(repaint_context->display, repaint_context->screen)); 

	/* We tell the window manager we want to handle Window Close Events ourselfs */
	/* FIXME!!! Dunno how this works. ...but it does work! */
	Atom delWindow = XInternAtom(repaint_context->display, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(repaint_context->display, repaint_context->window, &delWindow, 1);

	repaint_context->visual = DefaultVisual(repaint_context->display, repaint_context->screen);
	if(repaint_context->visual->class != TrueColor)
	{
		printf("Your display doesn't support TrueColor!\n");
		exit(1);
	}

	repaint_context->canvases = malloc(MAX_CANVASES * sizeof(XImage *));
	for(int i = 0; i < MAX_CANVASES; ++i)
		repaint_context->canvases[i] = malloc(sizeof(XImage));

	repaint_context->number_of_canvases = 1;
	repaint_context->active_canvas = 0;
	repaint_context->canvas = repaint_context->canvases[0];

	/* Initialize the colors */
	repaint_context->colors = malloc(NUMBER_OF_COLORS * sizeof(int));
	for(int i = 0; i < NUMBER_OF_COLORS; ++i)
		repaint_context->colors[i] = i*i* 0x80; /* Default to some random crappy colors */
	repaint_context->colors[0] = 0x00000000; /* white */
	repaint_context->colors[1] = 0x00FFFFFF; /* white */
	repaint_context->colors[2] = 0x00FF0000; /* red */
	repaint_context->colors[3] = 0x0000FF00; /* green */
	repaint_context->colors[4] = 0x000000FF; /* blue */
	repaint_context->colors[5] = 0x00FFFF00; /* yellow */
	repaint_context->colors[6] = 0x00FF00FF; /* magneta */
	repaint_context->colors[7] = 0x0000FFFF; /* cyan */
	repaint_context->active_color = 4; /* Start with the blue color active */
	return repaint_context;
}	

tool_contexts_t *init_tool_contexts(repaint_context_t *repaint_context)
{
	/* Initialize the line tool */
	tool_line_context_t *tool_line_context = malloc(sizeof(tool_line_context_t));
	tool_line_context->is_painting = False;
	tool_line_context->repaint_context = repaint_context;

	/* Initialize the freehand tool */
	tool_freehand_context_t *tool_freehand_context = malloc(sizeof(tool_freehand_context_t));
	tool_freehand_context->repaint_context = repaint_context;
	tool_freehand_context->is_painting = False;
	tool_freehand_context->head = malloc(sizeof(ll_point_t));
	tool_freehand_context->head->next = NULL;

	/* Initialize the rectangle tool */
	tool_rectangle_context_t *tool_rectangle_context = malloc(sizeof(tool_rectangle_context_t));
	tool_rectangle_context->is_painting = False;
	tool_rectangle_context->repaint_context = repaint_context;

	/* Initialize the circle tool */
	tool_circle_context_t *tool_circle_context = malloc(sizeof(tool_circle_context_t));
	tool_circle_context->is_painting = False;
	tool_circle_context->repaint_context = repaint_context;

	/* Initialize the text tool */
	tool_text_context_t *tool_text_context = malloc(sizeof(tool_text_context_t));
//	tool_text_context->string = "Text";
	tool_text_context->repaint_context = repaint_context;

	/* Initalize a tool context struct that is used in the event loop */
	tool_contexts_t *tool_contexts = malloc(sizeof(tool_contexts_t));
	tool_contexts->tool_freehand_context = tool_freehand_context;
	tool_contexts->tool_line_context = tool_line_context;
	tool_contexts->tool_rectangle_context = tool_rectangle_context;
	tool_contexts->tool_circle_context = tool_circle_context;
	tool_contexts->tool_text_context = tool_text_context;
	return tool_contexts;
}

/* Copy the current canvas into a newly created canvas */
void copy_canvas(repaint_context_t *context)
{
	printf("copy canvas!\n");
	if(context->number_of_canvases >= MAX_CANVASES)
		return;

	char *imageInitData = malloc(context->canvas->width * context->canvas->height * sizeof(int));
	int *p_new = (int *) imageInitData;
	int *p_old = (int *) context->canvas->data;
	for(int i = 0; i < context->canvas->width * context->canvas->height; ++i)
		*p_new++ = *p_old++;

	context->canvases[context->number_of_canvases] = XCreateImage(context->display,
		context->visual, DEFAULT_DEPTH, ZPixmap, 0, imageInitData,
		context->canvas->width, context->canvas->height, 32, 0);

	context->active_canvas = context->number_of_canvases; 
	context->canvas = context->canvases[context->active_canvas];
	context->number_of_canvases++;
}

/* Note that the case when number_of_canvases == 0 is also handled well by this function */
void create_canvas(repaint_context_t *context, int width, int height)
{
	if(context->number_of_canvases >= MAX_CANVASES)
		return;

	char *imageInitData = malloc(width * height * sizeof(int));
	int *p = (int *) imageInitData;
	for(int i = 0; i < width * height; ++i)
		*p++ = 0x00000000; // Initialize canvas with black background

	context->canvases[context->number_of_canvases] = XCreateImage(context->display,
		context->visual, DEFAULT_DEPTH, ZPixmap, 0, imageInitData, width, height, 32, 0);
	context->active_canvas = context->number_of_canvases; 
	context->canvas = context->canvases[context->active_canvas];
	context->number_of_canvases++;
}


void delete_canvas(repaint_context_t *context)
{
	if(context->active_canvas == 0 || context->number_of_canvases <= 1)
		return;

	XDestroyImage(context->canvases[context->active_canvas]);
	--context->number_of_canvases;

	/* Rearrange the canvases pointers */
	for(int i = 0; i < (int) context->number_of_canvases; ++i)
	{
		if(i >= (int) context->active_canvas)
		context->canvases[i] = context->canvases[i+1];
	}

	if(context->active_canvas == context->number_of_canvases)
		--context->active_canvas;
	context->canvas = context->canvases[context->active_canvas];
}


int handle_xevent(repaint_context_t *repaint_context, tool_contexts_t *tool_contexts)
{
	XEvent event;
	while(XPending(repaint_context->display) != 0)
	{
		XNextEvent(repaint_context->display, &event);

		if(event.type == Expose)
			repaint(repaint_context);

		/* Did the window manager request to kill us? */
		else if(event.type == ClientMessage)
			printf("Press q to quit!\n");

		else if(event.type == KeyPress && repaint_context->active_window == NORMAL &&
				event.xkey.keycode == 24) /* q */
		{
			return 1;
		}

		else if(event.type == ConfigureNotify)
		{
			printf("Window size is now: %d, %d\n",
					event.xconfigure.width, event.xconfigure.height);
			repaint(repaint_context);
		}

		switch(repaint_context->active_window)
		{
			case NORMAL:
				handle_event_normal(repaint_context, tool_contexts, event);
				break;
			case HELP:
				handle_event_helpscreen(repaint_context, event);
				break;
			case COLOR:
				handle_event_colorscreen(repaint_context, event);
				break;
			case TEXT_INPUT:
				handle_event_text_input(repaint_context, event);
				break;
			default:
			printf("No active window?!\n");
			break;
			
		}
	}

	return 0;
}

void handle_event_text_input(repaint_context_t *repaint_context, XEvent event)
{
	if(event.type != KeyPress)
		return;
	if(event.xkey.keycode == 36) /* Enter pressed, we're done! */
	{
		repaint_context->active_window = NORMAL;
		repaint(repaint_context);
		return;
	}

	/* Check which character was pressed */
	int char_lookup[100] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0 - 9 */
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', /* 10 - 19 */
		0, 0, 0, 0, 'q', 'w', 'e', 'r', 't', 'y', /* 20 - 29 */
		'u', 'i', 'o', 'p', 0, 0, 0, 0, 'a', 's', /* 30 - 39 */
		'd', 'f', 'g', 'h', 'j', 'k', 'l', 0, 0, 0, /* 40 - 49 */
		0, 0, 'z', 'x', 'c', 'v', 'b', 'n', 'm', 0, /* 50 - 59 */
		'.', '-', 0, 0, 0, ' ', 0, 0, 0, 0 }; /* 60 - 69 */
	int key = event.xkey.keycode;
	if(key >= 100)
	{
		printf("key too large!\n");
		return;
	}
	else if(key == 22) /* backspace */
	{
		if(strlen(repaint_context->input_string) < 1)
			return;
		printf("Deleting a character\n");
		char *string = calloc(sizeof(char), strlen(repaint_context->input_string));
		strncpy(string, repaint_context->input_string, strlen(repaint_context->input_string) - 1);
		free(repaint_context->input_string);
		repaint_context->input_string = string;
	}
	else if(char_lookup[key] == 0)
	{
		printf("Key not relevant\n");
		return;
	}
	else
	{
		printf("Adding key: %c\n", key);
		char *string = malloc(sizeof(char) * (strlen(repaint_context->input_string) + 2));
		strcpy(string, repaint_context->input_string);
		string[strlen(repaint_context->input_string)] = char_lookup[key];
		string[strlen(repaint_context->input_string) + 1] = '\0';
		free(repaint_context->input_string);
		repaint_context->input_string = string;
	}
	repaint(repaint_context);
}

void handle_event_helpscreen(repaint_context_t *context, XEvent event)
{
	if(event.type != KeyPress)
		return;
	if(event.type == KeyPress)
		context->active_window = NORMAL;
	repaint(context);
}

void handle_event_colorscreen(repaint_context_t *context, XEvent event)
{

	if(event.type != KeyPress)
		return;

	int color = context->colors[context->active_color];
	if(event.xkey.keycode == 24) /* q */
		context->active_window = NORMAL;

	if(event.xkey.keycode == 38) /* a */
	{
		int c = color;
		c &= 0x00FF0000;
		c = c >> 16;
		c++;
		c = c << 16;
		color &= 0xFF00FFFF;
		c &= 0x00FF0000;
		color |= c;
	}
	if(event.xkey.keycode == 52) /* z */
	{
		int c = color;
		c &= 0x00FF0000;
		c = c >> 16;
		c--;
		c = c << 16;
		color &= 0xFF00FFFF;
		c &= 0x00FF0000;
		color |= c;
	}
	if(event.xkey.keycode == 39) /* s */
	{
		int c = color;
		c &= 0x0000FF00;
		c = c >> 8;
		c++;
		c = c << 8;
		color &= 0xFFFF00FF;
		c &= 0x0000FF00;
		color |= c;
	}
	if(event.xkey.keycode == 53) /* x */
	{
		int c = color;
		c &= 0x0000FF00;
		c = c >> 8;
		c--;
		c = c << 8;
		color &= 0xFFFF00FF;
		c &= 0x0000FF00;
		color |= c;
	}
	if(event.xkey.keycode == 40) /* d */
	{
		int c = color;
		c &= 0x000000FF;
		c++;
		color &= 0xFFFFFF00;
		c &= 0x000000FF;
		color |= c;
	}
	if(event.xkey.keycode == 54) /* c */
	{
		int c = color;
		c &= 0x000000FF;
		c--;
		color &= 0xFFFFFF00;
		c &= 0x000000FF;
		color |= c;
	}
	context->colors[context->active_color] = color;
	repaint(context);
}

void handle_event_normal(repaint_context_t *repaint_context, tool_contexts_t *tool_contexts, XEvent event)
{

	/* Did a mouse press happen within the button frame? */
	if(event.type == ButtonPress && event.xbutton.y < BUTTON_FRAME_DEFAULT_HEIGHT)
	{
		/* Tool 0? */
		if(event.xbutton.x < (int) repaint_context->window_width / NUMBER_OF_TOOLS)
		{
			/* Only repaint window if really needed! */
			if(repaint_context->active_tool != FREEHAND)
			{
				repaint_context->active_tool = FREEHAND;
				repaint(repaint_context);
			}
		} else if (event.xbutton.x < 2 * (int) repaint_context->window_width / NUMBER_OF_TOOLS)
		{
			/* Only repaint window if really needed! */
			if(repaint_context->active_tool != LINE)
			{
				repaint_context->active_tool = LINE;
				repaint(repaint_context);
			}
		} else if(event.xbutton.x < 3 * (int) repaint_context->window_width / NUMBER_OF_TOOLS)
		{
			/* Only repaint window if really needed! */
			if(repaint_context->active_tool != RECTANGLE)
			{
				repaint_context->active_tool = RECTANGLE;
				repaint(repaint_context);
			}
		} else if(event.xbutton.x < 4 * (int) repaint_context->window_width / NUMBER_OF_TOOLS)
		{
			/* Only repaint window if really needed! */
			if(repaint_context->active_tool != CIRCLE)
			{
				repaint_context->active_tool = CIRCLE;
				repaint(repaint_context);
			}
		} else if(event.type == ButtonPress)
		{
			repaint_context->active_window = TEXT_INPUT;
			repaint_context->active_tool = TEXT;
			repaint(repaint_context);
		}
	}

	/* Mouse press events within canvas,
	   or mouse release or mouse move events anywhere are sent to the active tool */
	else if((event.type == ButtonPress && event.xbutton.y < 
			(int) repaint_context->window_height -
		        (COLOR_FRAME_DEFAULT_HEIGHT + STATUS_FRAME_DEFAULT_HEIGHT)) ||
		        event.type == ButtonRelease || event.type == MotionNotify)
	{
		canvas_event_t canvas_event;
		canvas_event.type = event.type;
		canvas_event.x = event.xbutton.x;
		/* Adjust the coordinate to be relative to canvas */
		canvas_event.x = event.xbutton.x - CANVAS_FRAME_WIDTH + repaint_context->scroll_x;
		canvas_event.y = event.xbutton.y + repaint_context->scroll_y -
		       	(BUTTON_FRAME_DEFAULT_HEIGHT + CANVAS_FRAME_WIDTH);
		switch(repaint_context->active_tool) {
			case LINE:
				tool_line(tool_contexts->tool_line_context, canvas_event);
				break;
			case FREEHAND:
				tool_freehand(tool_contexts->tool_freehand_context, canvas_event);
				break;
			case RECTANGLE:
				tool_rectangle(tool_contexts->tool_rectangle_context, canvas_event);
				break;
			case CIRCLE:
				tool_circle(tool_contexts->tool_circle_context, canvas_event);
				break;
			case TEXT:
				tool_text(tool_contexts->tool_text_context, canvas_event);
				break;
			default:
				printf("No valid tool active!\n");
				break;
		}
		/* Only if the mouse button was released can something have changed on canvas */
		if(canvas_event.type == ButtonRelease)
			repaint(repaint_context);
	}
	/* Did the mouse button press occour within the color panel? */
	else if((event.type == ButtonPress) && (event.xbutton.y >
				(int) repaint_context->window_height -
		       	(COLOR_FRAME_DEFAULT_HEIGHT + STATUS_FRAME_DEFAULT_HEIGHT)) &&
			event.xbutton.y < 
			(int) repaint_context->window_height - STATUS_FRAME_DEFAULT_HEIGHT)
	{
		int column = (event.xbutton.x * NUMBER_OF_COLORS)
			/ (repaint_context->window_width * COLOR_ROWS);
		int row = event.xbutton.y - (repaint_context->window_height -
			(STATUS_FRAME_DEFAULT_HEIGHT + COLOR_FRAME_DEFAULT_HEIGHT));
		row = (row * COLOR_ROWS) / (COLOR_FRAME_DEFAULT_HEIGHT);
		int color = column * COLOR_ROWS + row;
		if(event.xbutton.button == 1) /* left button */
			repaint_context->active_color = color;
		else if(event.xbutton.button == 3) /* right button */
		{
			repaint_context->active_color = color;
			repaint_context->active_window = COLOR;
		}
		repaint(repaint_context);
	}
	
	/* Handle keyboard input */
	if (event.type == KeyPress)
	{
		printf("Key %d pressed!\n", event.xkey.keycode);
		if(event.xkey.keycode == 52) /* z */
		{
			if(repaint_context->thickness > 1)
				repaint_context->thickness -=1;
		}
		else if(event.xkey.keycode == 38)  /* x */
		{
			if(repaint_context->thickness < 99)
				repaint_context->thickness +=1;
		}
		else if(event.xkey.keycode == 43) /* h */
		{
			repaint_context->active_window = HELP;
		}
		else if(event.xkey.keycode == 57) /* n - create new canvas */
		{
			create_canvas(repaint_context, CANVAS_DEFAULT_WIDTH, CANVAS_DEFAULT_HEIGHT);
			repaint_context->scroll_x = 0;
			repaint_context->scroll_y = 0;
		}
		else if(event.xkey.keycode == 54) /* c - copy canvas */
		{
			copy_canvas(repaint_context);
			repaint_context->scroll_x = 0;
			repaint_context->scroll_y = 0;
		}
		else if(event.xkey.keycode == 40) /* d - delete canvas */
		{
			delete_canvas(repaint_context);
			repaint_context->scroll_x = 0;
			repaint_context->scroll_y = 0;
		}
		else if(event.xkey.keycode == 39) /* s - save canvas */
		{
			save_file(repaint_context);
		}
		/* 0-9 - switch to canvas */
		else if((event.xkey.keycode >= 10 && event.xkey.keycode < 20) && 
	       			(event.xkey.keycode - 9) <= repaint_context->number_of_canvases){
			repaint_context->active_canvas = event.xkey.keycode - 10;
			repaint_context->canvas = repaint_context->canvases[repaint_context->active_canvas];
			repaint_context->scroll_x = 0;
			repaint_context->scroll_y = 0;
		}
		/* PgUp - next canvas */
		else if(event.xkey.keycode == 112 &&
				repaint_context->active_canvas < 
				repaint_context->number_of_canvases - 1)
		{
			repaint_context->active_canvas += 1;
			repaint_context->canvas = repaint_context->canvases[repaint_context->active_canvas];
			repaint_context->scroll_x = 0;
			repaint_context->scroll_y = 0;
		}
		/* PgDn - previous canvas */
		else if((event.xkey.keycode == 117) &&
				repaint_context->active_canvas > 0 &&
				repaint_context->number_of_canvases > 1)
		{
			repaint_context->active_canvas -= 1;
			repaint_context->canvas = repaint_context->canvases[repaint_context->active_canvas];
			repaint_context->scroll_x = 0;
			repaint_context->scroll_y = 0;
		}
		/* up arrow - scroll up */
		else if(event.xkey.keycode == 111)
		{
			scroll_up(repaint_context);
		}
		/* down arrow - scroll down */
		else if(event.xkey.keycode == 116)
		{
			scroll_down(repaint_context);
		}
		/* left arrow - scroll left */
		else if(event.xkey.keycode == 113)
		{
			scroll_left(repaint_context);
		}
		/* right arrow - scroll right */
		else if(event.xkey.keycode == 114)
		{
			scroll_right(repaint_context);
		}
		repaint(repaint_context);
	}

}

void scroll_right(repaint_context_t *context)
{

	/* scroll_x must not be larger than max_width */
	int max_width = context->window_width - 2 * CANVAS_FRAME_WIDTH;

	/* If canvas is smaller than the width of the screen, don't scroll */
	if(context->canvas->width < max_width)
	{
		/* Don't do anything */
	}
	/* Are we getting too far to the right? */
	else if((int) (context->scroll_x + SCROLL_STEPS + max_width) > context->canvas->width)
	{
		context->scroll_x = context->canvas->width - max_width;
	}
	/* Everything's alright, scroll right one step */
       	else
	{
		context->scroll_x += SCROLL_STEPS;
	}

}


void scroll_left(repaint_context_t *context)
{

	/* If we're getting too close to the left, just set scroll_x to zero */
	if(context->scroll_x < SCROLL_STEPS)
	{
		context->scroll_x = 0;
	}
	/* Otherwise just scroll one step to the left */
	else
	{
		context->scroll_x -= SCROLL_STEPS;
	}

}


void scroll_down(repaint_context_t *context)
{

	/* scroll_x must not be larger than max_width */
	int max_height = context->window_height -
		(2 * CANVAS_FRAME_WIDTH + BUTTON_FRAME_DEFAULT_HEIGHT);

	/* If canvas is smaller than the height of the screen, don't scroll */
	if(context->canvas->height < max_height)
	{
		/* Don't do anything */
	}
	/* Are we getting too far to the top? */
	else if((int) (context->scroll_y + SCROLL_STEPS + max_height) > context->canvas->height)
	{
		context->scroll_y = context->canvas->height - max_height;
	}
	/* Everything's alright, scroll down one step */
       	else
	{
		context->scroll_y += SCROLL_STEPS;
	}


}

void scroll_up(repaint_context_t *context)
{

	/* If we're getting too close to the top, just set scroll_y to zero */
	if(context->scroll_y < SCROLL_STEPS)
	{
		context->scroll_y = 0;
	}
	/* Otherwise just scroll one step to the left */
	else
	{
		context->scroll_y -= SCROLL_STEPS;
	}

}

