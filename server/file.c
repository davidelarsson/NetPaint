#include <png.h>
#include <stdlib.h>
#include "file.h"

char* get_filename()
{
	FILE *fp = NULL;
	char *name_tmp = "NetPaint0000.png";
	char *name = malloc(sizeof(char) * strlen(name_tmp) + 1);
	strcpy(name, name_tmp);
	int i = 0;
	while((fp = fopen(name, "r")) != NULL)
	{
		fclose(fp);
		sprintf(name, "NetPaint%04i.png", i++);
	}
	return name;
}


void save_file(connections_t *connections)
{
	char *filename = get_filename();
	int width = connections->canvas_w;
	int height = connections->canvas_h;
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;

	fp = fopen(filename, "wb");
	if(fp == NULL)
	{
		printf("ERROR! Could not open file!\n");
	//	return -1;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		printf("ERROR! Could not allocate write structure!\n");
	//	return -1;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL)
	{
		printf("ERROR! Could not allocate info structure!\n");
	//	return -1;
	}


	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "Error during png creation\n");
	//	return -1;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	png_text title_text;
	title_text.compression = PNG_TEXT_COMPRESSION_NONE;
	title_text.key = "Title";
	title_text.text = "NetPaint canvas";
	png_set_text(png_ptr, info_ptr, &title_text, 1);

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for(y=0 ; y<height ; y++) {
		for (x=0 ; x<width ; x++) {
			row[x*3 + 2] = connections->canvas[4 * (y * width + x)];
			row[x*3 + 1] = connections->canvas[4 * (y * width + x) + 1];
			row[x*3] = connections->canvas[4 * (y * width + x) + 2];
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

}
