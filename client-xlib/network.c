#include "network.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <zlib.h>
#include <X11/Xlib.h>
#include "xlib.h"

int init_network(char* address, int port)
{

	struct addrinfo hints;
	struct addrinfo *res;
	int sfd;

	// first, load up address structs with getaddrinfo():

	memset(&hints, 0, sizeof(struct addrinfo)); /* All hints not set must be zeroed out */
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	char* str_port = malloc(sizeof(char) * 6);
	sprintf(str_port, "%d", port);
	int rv;
	if((rv = getaddrinfo(address, str_port, &hints, &res) != 0))
	{
		printf("ERROR! Could not get address info: %s\n", gai_strerror(rv));
		return -1;
	}

	sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if(connect(sfd, res->ai_addr, res->ai_addrlen) == -1)
	{
		printf("ERROR! Could not connect: %s\n", strerror(errno));
		return -1;
	}

	freeaddrinfo(res);

	printf("Connected to: %s:%d\n", address, port);

	return sfd;
}

int ready_read(int sfd, repaint_context_t *repaint_context)
{
	char *rec_buf = calloc(sizeof(char), 4);
	int *ptr = (int *) rec_buf;

	/* First is the message size */
	recv(sfd, rec_buf, 4, 0);
	int message_size = ntohl(ptr[0]);
	printf("Message size: %d\n", message_size);

	/* Protocol number */
	recv(sfd, rec_buf, 4, 0);
	int protocol = ntohl(ptr[0]);
	printf("Protocol: 0x%02x\n", protocol);

	/* Canvas width */
	recv(sfd, rec_buf, 4, 0);
	int canvas_width = ntohl(ptr[0]);
	printf("Canvas width: %d\n", canvas_width);

	/* Canvas height */
	recv(sfd, rec_buf, 4, 0);
	int canvas_height = ntohl(ptr[0]);
	printf("Canvas height: %d\n", canvas_height);

	/* The canvas itself */
	int received_bytes;
	free(rec_buf);

	rec_buf = calloc(sizeof(char), message_size - 16);
	/*
	received_bytes = recv(sfd, rec_buf, message_size, 0);
	*/
	int tmp_var = message_size - 16;
	int *len = &tmp_var;
        if((received_bytes = get_all(sfd, rec_buf, len)) == -1)
        {
                printf("Error while reading data! Only read %d bytes out of %d", received_bytes, message_size);
        }

	printf("Received %d bytes of canvas.\n", *len);
	received_bytes = *len;

	/* FIXME!!! Is this really sufficient? */
	uLong decomp_buffer_length = (canvas_width * canvas_height * 4);
	char *decomp_buffer = malloc(sizeof(char) * decomp_buffer_length);
	for(int i = 0; i < (int) decomp_buffer_length; ++i)
		decomp_buffer[i] = 0x00000000;

	int uncomp_res = uncompress((Bytef *) decomp_buffer, &decomp_buffer_length, (Bytef *) rec_buf + 4, received_bytes);
	printf("decomp_buffer_length is now: %d\n", (int) decomp_buffer_length);

	switch(uncomp_res)
	{
		case Z_MEM_ERROR:
			printf("zlib memory error!\n");
			return -1;
		case Z_BUF_ERROR:
			printf("zlib buffer error!\n");
			return -1;
		case Z_DATA_ERROR:
			printf("zlib data error!\n");
			return -1;
		default:
			printf("zlib uncompress unknown error: %d", uncomp_res);
			return -1;
		case Z_OK:
			printf("Decompressed: %d bytes!\n", (int) decomp_buffer_length);
	}

	/* Replace the old canvas with the new one */
	XImage *image = malloc(sizeof(XImage));
	image = XCreateImage(repaint_context->display, repaint_context->visual, DEFAULT_DEPTH, ZPixmap, 0,
			decomp_buffer, canvas_width, canvas_height, 32, 0);

//	XImage *old_canvas = repaint_context->canvas;
	repaint_context->canvases[0] = image; /* The first canvas is the network enabled one */
	repaint_context->canvas = repaint_context->canvases[0];
//	XDestroyImage(old_canvas);

	return 0;
}

int get_all(int sfd, char *buffer, int *len)
{

    int total = 0;        // how many bytes we've received
    int bytesleft = *len; // how many we have left to send
    int bytes_received;

    while(total < *len) {
	//received_bytes = recv(sfd, rec_buf, message_size, 0);
        bytes_received = recv(sfd, buffer + total, bytesleft, 0);
        if (bytes_received == -1)
        {
                printf("Error while sending data!");
                return -1;
        }
        total += bytes_received;
        bytesleft -= bytes_received;
    }

    *len = total; // return number actually sent here

    return (bytes_received == -1) ? -1 : 0; // return -1 on failure, 0 on success

}


void send_image(XImage *image, repaint_context_t *repaint_context)
{


        printf("**************** send_image() ******************\n");
        unsigned canvas_size = repaint_context->canvas->width * repaint_context->canvas->height;
        /* FIXME!!! This is waaaay too much... but I dunno how to calculate a good value */
        char *send_buffer = calloc(sizeof(int), canvas_size + 20);
        uLong compressed_canvas_size = canvas_size * 4;
        printf("total size of compressed canvas buffer is now: %d\n", (int) compressed_canvas_size);
        int comp_res;
        if((comp_res = compress((Bytef *) send_buffer + 20, &compressed_canvas_size,
                                (Bytef *) image->data, canvas_size * 4)) != Z_OK)
        {
                printf("zlib compress error: %d", comp_res);
                exit(1);
        }
        printf("Compressed to: %d bytes!\n", (int) compressed_canvas_size);

        int *p = (int *) send_buffer;
        int packet_length = compressed_canvas_size + 20;
        *p++ = htonl(packet_length);
        int protocol_number = 0x00000002;
        *p++ = htonl(protocol_number);
        *p++ = htonl(repaint_context->canvas->width);
        *p++ = htonl(repaint_context->canvas->height);
        *p++ = htonl(canvas_size); /* FIXME: This shit ain't workin' */

        int bytes_sent = 0;
	/*
        if((bytes_sent = send(repaint_context->sfd, send_buffer, packet_length, 0)) < 0)
                printf("send error: %s\n", strerror(errno));
	*/
        int *len = &packet_length;
        if((bytes_sent = send_all(repaint_context->sfd, send_buffer, len)) == -1)
        {
                printf("Error while sending data! Only %d bytes sent out of %d", bytes_sent, packet_length);
        }

        printf("Sent %d bytes of canvas\n", bytes_sent);
	printf("******************* end of send_image() *********************\n");


}

int send_all(int sfd, char *buffer, int *len)
{

    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int bytes_sent;

    while(total < *len) {
        bytes_sent = send(sfd, buffer + total, bytesleft, 0);
        if (bytes_sent == -1)
        {
                printf("Error while sending data!");
                return -1;
        }
        total += bytes_sent;
        bytesleft -= bytes_sent;
    }

    *len = total; // return number actually sent here

    return (bytes_sent == -1) ? -1 : 0; // return -1 on failure, 0 on success

}
 
