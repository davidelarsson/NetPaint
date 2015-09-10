#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <zlib.h>
#include "server.h"

int create_server_socket(char *address, int port)
{
	/* getaddrinfo takes a string. I thought it'd look better to call this function with an int */
	char port_str[6];
	sprintf(port_str, "%d", port);

	/* Get all the IPv4 addresses associated with 'localhost' */
	struct addrinfo hints;
	struct addrinfo *addrinfo_res, *rp;
	memset(&hints, 0, sizeof(struct addrinfo)); /* Make sure all fields we don't use are zeroed out */
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	char *node = address;
	int status;
	if((status = getaddrinfo(node, port_str, &hints, &addrinfo_res)) != 0)
	{
		fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status));
		return(-1);
	}

	char ipstr[INET6_ADDRSTRLEN]; /* In case we want to make it IPv6 compatible some day */
	for(rp = addrinfo_res; rp != NULL; rp = rp->ai_next)
	{
		struct sockaddr_in *ip = (struct sockaddr_in *) rp->ai_addr;
		inet_ntop(AF_INET, &(ip->sin_addr), ipstr, sizeof ipstr);
		//printf("And we shall use: %s\n", ipstr);
	}
	int sfd;
	if((sfd = socket(addrinfo_res->ai_family, addrinfo_res->ai_socktype, addrinfo_res->ai_protocol)) < 0)
	{
		fprintf(stderr, "Socket error: %s\n", strerror(errno));
		return(-1);
	}
	int yes = 1;
	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
	{
		fprintf(stderr, "setsocketopt() failed: %s\n", strerror(errno));
		return(-1);
	}
	if((bind(sfd, addrinfo_res->ai_addr, addrinfo_res->ai_addrlen)) < 0)
	{
		fprintf(stderr, "Could not bind: %s\n", strerror(errno));
		return(-1);
	}
	if(listen(sfd, 2) < 0)
	{
		fprintf(stderr, "Could not listen: %s\n", strerror(errno));
		return(-1);
	}
	return sfd;

}


/* Call this whenever the canvas needs to be reinitialized.
   If called with argument NULL, a default canvas is created.
   If argument is not NULL, the canvas_w_tmp and canvas_h_tmp is used to
   adjust the current canvas */
connections_t *init_canvas(connections_t *prev)
{
        connections_t *con;

	/* If argument is NULL, just create a default canvas */
	if(prev == NULL)
	{
        	con = malloc(sizeof(connections_t));
	        con->connection = NULL;
		con->canvas_w = CANVAS_DEFAULT_WIDTH;
		con->canvas_h = CANVAS_DEFAULT_HEIGHT;
		con->canvas_w_tmp = con->canvas_w;
		con->canvas_h_tmp = con->canvas_h;
		con->canvas = malloc(sizeof(char) * con->canvas_w * con->canvas_h * 4);
		con->comp_canvas = NULL;
		con->comp_canvas_size = 0;
		con->connections_n = 0;
		con->err_list = NULL;
	
		/* ARGB demands that the most significant 8 bits out of the 32-bits int are set */
		for(int i = 3; i < (int) (con->canvas_w * con->canvas_h * 4); i += 4)
			con->canvas[i] = (char) 0xFF;
		
		/* Start with a nice background color */
		int *ptr = (int *) con->canvas;
		for(int i = 0; i < (int) (con->canvas_w * con->canvas_h); i++)
			ptr[i] |= DEFAULT_BACKGROUND_COLOR;
	}

       	/* If we were called with a valid connections argument, it means there's a previous
	   canvas available. Use that one to adjust the size of the new canvas */	   
	else {
		char *new_canvas = calloc(sizeof(char), prev->canvas_w_tmp * prev->canvas_h_tmp * 4);

		/* ARGB demands that the most significant 8 bits out of the 32-bits int are set */
		for(int i = 3; i < (int) (prev->canvas_w_tmp * prev->canvas_h_tmp * 4); i += 4)
			new_canvas[i] = (char) 0xFF;

		/* Start with a nice background color */
		int *new_ptr = (int *) new_canvas;
		for(int i = 0; i < (int) (prev->canvas_w_tmp * prev->canvas_h_tmp); i++)
			new_ptr[i] |= DEFAULT_BACKGROUND_COLOR;

		/* Then copy in as much as possible from the previous canvas to the new one */
		int *old_ptr = (int *) prev->canvas;
		for(int i = 0; i < (int) prev->canvas_h; ++i)
			if(i < (int) prev->canvas_h_tmp)
				for(int j = 0; j < (int) prev->canvas_w; ++j)
					if(j < (int) prev->canvas_w_tmp)
						new_ptr[i * prev->canvas_w_tmp + j] =
							old_ptr[i * prev->canvas_w  + j];

		/* Clean up the old canvas */
		free(prev->canvas);
		prev->canvas = NULL;
		prev->canvas_w = prev->canvas_w_tmp;
		prev->canvas_h = prev->canvas_h_tmp;
		free(prev->comp_canvas);
		prev->comp_canvas = NULL;
		prev->comp_canvas_size = 0;
		prev->canvas = new_canvas;

		con = prev;
	}
	compress_canvas(con);
	update_canvas(con);
	return con;

}

/* Add a string to the linked list of error strings in the connections_t struct */
void add_error_string(connections_t *connections, char* str)
{
	if(connections->err_list == NULL)
	{
		connections->err_list = malloc(sizeof(err_list_t));
		connections->err_list->next = NULL;
		connections->err_list->str = malloc(sizeof(char) * strlen(str) + 2);
		strcpy(connections->err_list->str, str);
		connections->err_list->str[strlen(str)] = '\n';
		connections->err_list->str[strlen(str) + 1] = '\0';
	} else {
		err_list_t *tmp = connections->err_list;
		while(tmp->next != NULL)
			tmp = (err_list_t *) tmp->next;
		tmp->next = malloc(sizeof(err_list_t));
		tmp = (err_list_t *) tmp->next;
		tmp->next = NULL;
		tmp->str = malloc(sizeof(char) * strlen(str) + 2);
		strcpy(tmp->str, str);
		tmp->str[strlen(str)] = '\n';
		tmp->str[strlen(str) + 1] = '\0';
	}
}


int handle_client(int sfd, connections_t *connections)
{

	/* FIXME!!! We need to check that the size of the image of the incoming
	   draw is equal to the size of the current canvas */

	//printf("**************** handle_connection() ******************\n");

	/* We receive the first 8 bytes */
	char *rec_buf = malloc(sizeof(char) * 4);
	int *ptr = (int *) rec_buf;

	int bytes_received = recv(sfd, rec_buf, 4, 0);
	if(bytes_received == 0)
	{
		/* If no bytes were received, it means the clent has closed the connection.
		   Inform the caller by returning -1
		*/
		return -1;
	}
	/* Otherwise, what we just received was the size of the package */
	int message_size = ntohl(*ptr);

	char tmpStr[100];
	sprintf(tmpStr, "Message size: %d", message_size);
	add_error_string(connections, tmpStr);

	recv(sfd, rec_buf, 4, 0);
	int protocol = ntohl(*ptr); /* Second byte is the protocol number */

	sprintf(tmpStr, "Protocol: %d", protocol);
	add_error_string(connections, tmpStr);

	free(rec_buf);

	switch(protocol)
	{
		case 0x00000002: /* Client has painted */
			client_paint(message_size, sfd, connections);
			break;

		default:
			//printf("Unsupported protocol!\n");
			return -1;
	}

	return 0;
}

int client_paint(int message_size, int sfd, connections_t *connections)
{
	/* Check to see that the client isn't write locked */
	connection_t *tmp_con = connections->connection;
	int i = 0;
	while(tmp_con != NULL)
	{
		if(tmp_con->fd == sfd && tmp_con->write_lock != 0)
		{
			int tmp_var = message_size - 8;
			int *len = &tmp_var;
			char *dump_buf = calloc(sizeof(char), message_size - 8);
        		get_all(sfd, dump_buf, len);
			free(dump_buf);
			return 0;
		}
		tmp_con = (connection_t *) tmp_con->next;
		i++;
	}

	char *rec_buf = malloc(sizeof(char) * 4);
	int *ptr = (int *) rec_buf;

	recv(sfd, rec_buf, 4, 0);
	int image_width = ntohl(*ptr); /* Second byte is the image width */
	char tmpStr[100];
	sprintf(tmpStr, "Canvas width: %d", image_width);
	add_error_string(connections, tmpStr);

	recv(sfd, rec_buf, 4, 0);
	int image_height = ntohl(*ptr); /* Second byte is the image height */
	sprintf(tmpStr, "Canvas height: %d", image_height);
	add_error_string(connections, tmpStr);

	free(rec_buf); /* Need more space for the image itself */
	rec_buf = calloc(sizeof(char), message_size - 16);
	int tmp_var = message_size - 16;
	int *len = &tmp_var;
	int bytes_received;
        if((bytes_received = get_all(sfd, rec_buf, len)) == -1)
        {
                //printf("Error while reading data! Only read %d bytes out of %d", bytes_received, message_size);
        }

	sprintf(tmpStr, "Received %d bytes of cavnas", *len);
	add_error_string(connections, tmpStr);
	bytes_received = *len;

	/* Now, increase the counter of bytes received from this client */
	for(connection_t *con = connections->connection; con != NULL; con = (connection_t *) con->next)
	{
		if(con->fd == sfd)
			con->rec_bytes += bytes_received;
	}
	



	/* Now, we have the compressed image in rec_buf */
	uLong decompressed_buffer_length = image_width * image_height * 4;
	char *decompressed_buffer = malloc(sizeof(char) * decompressed_buffer_length);

	sprintf(tmpStr, "Decompressing...");
	add_error_string(connections, tmpStr);
	int decomp_res = uncompress((Bytef *) decompressed_buffer, &decompressed_buffer_length,
			(Bytef *) rec_buf + 4, bytes_received);
        switch(decomp_res)
        {
                case Z_MEM_ERROR:
			sprintf(tmpStr, "zlib memory error!");
			add_error_string(connections, tmpStr);
                        return -1;
                case Z_BUF_ERROR:
			sprintf(tmpStr, "zlib buffer error!");
			add_error_string(connections, tmpStr);
                        return -1;
                case Z_DATA_ERROR:
			sprintf(tmpStr, "zlib data error!");
			add_error_string(connections, tmpStr);
                        return -1;
                default:
			sprintf(tmpStr, "Unknown zlib error: %d", decomp_res);
			add_error_string(connections, tmpStr);
                        return -1;
                case Z_OK:
			decomp_res++;
			decomp_res--;
			sprintf(tmpStr, "Successfully decompressed %d bytes!", (int) decompressed_buffer_length);
			add_error_string(connections, tmpStr);
        }
	/* Add the result string from the decompression */

	int *canvas_ptr = (int *) connections->canvas;
	int *buffer_ptr = (int *) decompressed_buffer;
	unsigned w = connections->canvas_w;
	unsigned h = connections->canvas_h;

	/* Update the canvas */
	for(int i = 0; i < (int) (w * h); ++i)
		if((buffer_ptr[i] & 0xFF000000) != 0x00000000)
			canvas_ptr[i] = buffer_ptr[i];
	compress_canvas(connections);

	return 0; /* sucess! */
}


int new_connection(connections_t *connections)
{
	//printf("**************** new_connection() ******************\n");
//	unsigned client_sockaddr_size;
//	struct sockaddr client_sockaddr;
//	int incoming_sfd = accept(connections->server_sfd, &client_sockaddr, &client_sockaddr_size);
	int incoming_sfd = accept(connections->server_sfd, NULL, NULL);
	if(incoming_sfd < 0)
	{
		//printf("accept() error: %s\n", strerror(errno));
		return -1;
	}
	add_connection(incoming_sfd, connections);
	send_canvas(incoming_sfd, connections);
	return incoming_sfd;
}

/* Dump connections */
/*
void print_connections(connections_t *connections)
{
	connection_t *con = connections->connection;
	while(con != NULL)
	{
		con = (connection_t *) con->next;
	}
}
*/
/* Add an fds to the connections list */
void add_connection(int new_fd, connections_t *connections)
{
	//printf("**************** add_connection() ******************\n");

	connection_t *tmp = malloc(sizeof(connection_t));
	tmp->fd = new_fd;
	tmp->sent_bytes = 0;
	tmp->rec_bytes = 0;
	tmp->write_lock = 0;
	connections->connections_n += 1;
	if(connections->connection == NULL)
	{
		connections->connection = tmp;
		connections->connection->next = NULL;
	} else
	{
		tmp->next = (void *) connections->connection;
		connections->connection = (connection_t *) tmp;
	}

	//printf("add_connection(): added %d\n", new_fd);
	return;

}

void delete_connection(int fd, connections_t *connections)
{

	//printf("**************** delete_connection() ******************\n");
	//printf("Trying to delete %d...\n", fd);
	connection_t *head = connections->connection;
	connection_t *tmp = head;
	connection_t *prev;
	connections->connections_n -= 1;

	while(tmp != NULL)
	{
		//printf("checking %d...\n", tmp->fd);
		if(tmp->fd == fd)
		{
			//printf("We found %d!\n", tmp->fd);
			if(tmp == head && tmp->next == NULL)
			{
				//printf("%d is the head, and alone!\n", tmp->fd);
				connections->connection = NULL;
				free(tmp);
				return;
			} else if(tmp == head && tmp->next != NULL)
			{
				//printf("%d is the head, but others follow!\n", tmp->fd);
				connections->connection = (connection_t *) tmp->next;
				free(tmp);
				return;
			}
			else
			{
				//printf("%d is somewhere in the list...\n", tmp->fd);
				prev->next = tmp->next;
				free(tmp);
				return;
			}
		}
		else
		{
			prev = tmp;
			tmp = (connection_t *) tmp->next;
		}
	}
}

void update_canvas(connections_t *connections)
{
	connection_t *con = connections->connection;
	//printf("**************** update_canvas() ******************\n");
	while(con != NULL)
	{
		//printf("Sending canvas to: %d\n", con->fd);
		send_canvas(con->fd, connections);
		con = (connection_t *) con->next;
	}
}


void compress_canvas(connections_t *connections)
{
	//printf("**************************** compress_canvas() *********************\n");
	unsigned canvas_size = connections->canvas_w * connections->canvas_h;
	/* FIXME!!! This is waaaay too much... but I dunno how to calculate a good value */
	char *tmp_buffer = calloc(sizeof(int), canvas_size);
	uLong compressed_canvas_size = canvas_size * 4;
	//printf("Before compression, canvas buffer is now: %d bytes long\n", (int) compressed_canvas_size);
	int comp_res;
	if((comp_res = compress((Bytef *) tmp_buffer, &compressed_canvas_size,
				(Bytef *) connections->canvas, connections->canvas_w
			       	* connections->canvas_h * 4)) != Z_OK)
	{
		//printf("zlib error: %d", comp_res);
		exit(-1);
        }

	char *tmp_ptr = connections->comp_canvas;
	connections->comp_canvas = malloc(sizeof(char) * compressed_canvas_size);
	free(tmp_ptr);

	memcpy(connections->comp_canvas, tmp_buffer, compressed_canvas_size);
	free(tmp_buffer);

	connections->comp_canvas_size = compressed_canvas_size;
	//printf("Compressed canvas to: %d bytes!\n", (int) compressed_canvas_size);
}


void send_canvas(int sfd, connections_t *connections)
{

	//printf("**************** send_canvas() ******************\n");
	unsigned canvas_size = connections->canvas_w * connections->canvas_h;
	int compressed_canvas_size = connections->comp_canvas_size;
	char *send_buffer = calloc(sizeof(char), connections->comp_canvas_size + 20);
	memcpy(send_buffer + 20, connections->comp_canvas, connections->comp_canvas_size);

	int *p = (int *) send_buffer;
	int packet_length = compressed_canvas_size + 20;
	*p++ = htonl(packet_length);
	int protocol_number = 0x00000002;
	*p++ = htonl(protocol_number);
	*p++ = htonl(connections->canvas_w);
	*p++ = htonl(connections->canvas_h);
	*p++ = htonl(canvas_size); /* FIXME: This shit ain't workin' */

	int bytes_sent = 0;
	int *len = &packet_length;
	if((bytes_sent = send_all(sfd, send_buffer, len)) == -1)
	{
		//printf("Error while sending data! Only %d bytes sent out of %d", bytes_sent, packet_length);
	}

	/* Now, increase the counter of bytes sent to this client */
	for(connection_t *con = connections->connection; con != NULL; con = (connection_t *) con->next)
	{
		if(con->fd == sfd)
			con->sent_bytes += packet_length;
	}
	

	//printf("Sent %d bytes of canvas\n", *len);

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
		//printf("Error while sending data!");
		return -1;
	}
        total += bytes_sent;
        bytesleft -= bytes_sent;
    }

    *len = total; // return number actually sent here

    return (bytes_sent == -1) ? -1 : 0; // return -1 on failure, 0 on success

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
                //printf("Error while sending data!");
                return -1;
        }
        total += bytes_received;
        bytesleft -= bytes_received;
    }

    *len = total; // return number actually sent here

    return (bytes_received == -1) ? -1 : 0; // return -1 on failure, 0 on success

}

