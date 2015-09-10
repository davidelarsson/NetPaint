#ifndef NETWORK_H
#define NETWORK_H

#define CANVAS_DEFAULT_WIDTH 800
#define CANVAS_DEFAULT_HEIGHT 600
#define DEFAULT_BACKGROUND_COLOR 0x0000FF00

typedef struct connection_s {
	int fd;
	unsigned rec_bytes;
	unsigned sent_bytes;
	int write_lock;
	struct connection_t *next;
} connection_t;

typedef struct err_list_s {
	char* str;
	struct err_list_t *next;
} err_list_t;

typedef struct connections_s {
	int server_sfd;
	char *canvas;
	char *comp_canvas;
	err_list_t *err_list;
	unsigned comp_canvas_size;
	unsigned canvas_w;
	unsigned canvas_h;
	unsigned canvas_w_tmp;
	unsigned canvas_h_tmp;
	connection_t *connection;
	unsigned connections_n;
} connections_t;

connections_t *init_canvas(connections_t *prev);
int create_server_socket(char* address, int port);
void compress_canvas(connections_t *connections);
void send_canvas(int fd, connections_t *connections);
void update_canvas(connections_t *connections);
int send_all(int sfd, char *buffer, int *len);
int get_all(int sfd, char *buffer, int *len);
int new_connection(connections_t *connections);
void add_connection(int new_fd, connections_t *connections);
void delete_connection(int fd, connections_t *connections);
int handle_client(int fd, connections_t *connections);
int client_paint(int message_size, int sfd, connections_t *connections);

#endif /* NETWORK_H */
