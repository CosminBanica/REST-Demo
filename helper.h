#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>

#define BUFLEN 4096
#define LINELEN 1000
#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

using namespace std;

typedef struct {
    char *data;
    size_t size;
} buffer;

buffer buffer_init(void);

void buffer_destroy(buffer *buffer);

void buffer_add(buffer *buffer, const char *data, size_t data_size);

int buffer_is_empty(buffer *buffer);

int buffer_find(buffer *buffer, const char *data, size_t data_size);

int buffer_find_insensitive(buffer *buffer, const char *data, size_t data_size);

void error(const char *msg);

void compute_message(char *message, const char *line);

void compute_cookie(char *message, char *line);

int open_connection(const char *host_ip, int portno, int ip_type, int socket_type, int flag);

void close_connection(int sockfd);

void send_to_server(int sockfd, char *message);

char *receive_from_server(int sockfd);

char *basic_extract_json_response(char *str);

char *compute_get_request(const char *host, const char *url, const char *query_params,
							char* cookies, string* token);

char *compute_post_request(const char *host, const char *url, const char* content_type, string body_data,
							char* cookies, string* token);

char *compute_delete_request(const char *host, const char *url, const char *query_params,
                            char* cookies, string* token);