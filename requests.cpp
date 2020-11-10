#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include "helper.h"

using namespace std;

char *compute_get_request(const char *host, const char *url, const char *query_params,
                            char* cookies, string* token)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    //Authorization: Bearer eijjkwuqioueu9182712093801293
    if (token != NULL) {
        strcat(message, "Authorization: Bearer ");
        compute_message(message, token->c_str());
    }

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies[0] != 0) {
        strcat(message, "Cookie: ");
        compute_message(message, cookies);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(const char *host, const char *url, const char* content_type, string body_data,
                            char *cookies, string* token)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    if (token != NULL) {
        strcat(message, "Authorization: Bearer ");
        compute_message(message, token->c_str());
    }


    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", body_data.length());
    compute_message(message, line);

    
    // Step 4 (optional): add cookies
    if (cookies != NULL && cookies[0] != 0) {
        strcat(message, "Cookie: ");
        compute_message(message, cookies);
    }
    
    // Step 5: add new line at end of header
    compute_message(message, "");
    // Step 6: add the actual payload data

    compute_message(message, body_data.c_str());

    free(line);
    return message;
}

char *compute_delete_request(const char *host, const char *url, const char *query_params,
                            char* cookies, string* token)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    //Authorization: Bearer eijjkwuqioueu9182712093801293
    if (token != NULL) {
        strcat(message, "Authorization: Bearer ");
        compute_message(message, token->c_str());
    }

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies[0] != 0) {
        strcat(message, "Cookie: ");
        compute_message(message, cookies);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}