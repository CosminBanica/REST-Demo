#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include "helper.h"
#include "json.hpp"

using namespace nlohmann;
using namespace std;

#define IP_SERVER "3.8.116.10"
#define PORT_SERVER 8080

int main() {
    char *message;
    char *response;
    int sockfd;

    string comm;
    char actual_cookie[1000];
    string* jwt;
    jwt = NULL;
    memset(actual_cookie, 0, 1000);
    actual_cookie[0] = 0;

    cout << "use \"help\" for list of commands\n";

    while(1) {
        getline(std::cin, comm);

        if (comm == "help") {
            cout << "\n-- List of commands:\n";
            cout << "register (+ username, + password)\n";
            cout << "login (+ username, + password)\n";
            cout << "enter_library\n";
            cout << "get_books\n";
            cout << "get_book (+ id)\n";
            cout << "add_book (+ title, + author, + genre, + publisher, + page_count)\n";
            cout << "delete_book (+ id)\n";
            cout << "logout\n";
            cout << "exit\n\n";
            continue;
        } else

        if (comm == "register") {
            string user, pass;
            printf("username:");
            getline(std::cin, user);
            printf("password:");
            getline(std::cin, pass);

            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            json j;
            j["username"] = user;
            j["password"] = pass;
            string data = j.dump();

            message = compute_post_request(IP_SERVER, "/api/v1/tema/auth/register",
            "application/json", data, NULL, NULL);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char cpy[1000];
            strcpy(cpy, response);
            char* code;
            code = strtok(cpy, " ");
            code = strtok(NULL, " ");

            if (!strcmp(code , "400")) {
                printf("400 Bad Request\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }
            
            printf("Successfully registered!\n\n");
            continue;
        } else

        if (comm == "login") {
            if (actual_cookie[0] != 0) {
                printf("Already logged in!\n\n");
                continue;
            }

            string user, pass;
            printf("username:");
            getline(std::cin, user);
            printf("password:");
            getline(std::cin, pass);

            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            json j;
            j["username"] = user;
            j["password"] = pass;
            string data = j.dump();

            message = compute_post_request(IP_SERVER, "/api/v1/tema/auth/login",
            "application/json", data, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char cpy[1000];
            strcpy(cpy, response);
            char* code;
            code = strtok(cpy, " ");
            code = strtok(NULL, " ");

            if (!strcmp(code , "400")) {
                printf("400 Bad Request\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            char *cookie;
            cookie = strstr(response, "Set-Cookie: ");
            cookie += 12;
            char *aux;
            aux = strchr(cookie, ';');
            memcpy(actual_cookie, cookie, aux - cookie);

            printf("Successfully logged in!\n\n");
            continue;
        } else

        if (comm == "enter_library") {
            jwt = NULL;

            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(IP_SERVER, "/api/v1/tema/library/access", 
            NULL, actual_cookie, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char cpy[1000];
            strcpy(cpy, response);
            char* code;
            code = strtok(cpy, " ");
            code = strtok(NULL, " ");

            if (!strcmp(code , "400")) {
                printf("400 Bad Request\n");
                printf("Couldn't prove logged in.\n\n");

                continue;
            }

            if (!strcmp(code , "401")) {
                printf("401 Unauthorized\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            string resp;
            resp = strstr(response, "\r\n\r\n");
            json jResp = json::parse(resp);
            jwt = new string(jResp["token"]);

            printf("Successfully entered library!\n\n");
            continue;
        } else

        if (comm == "get_books") {
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(IP_SERVER, "/api/v1/tema/library/books", 
            NULL, NULL, jwt);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char cpy[1000];
            strcpy(cpy, response);
            char* code;
            code = strtok(cpy, " ");
            code = strtok(NULL, " ");

            if (!strcmp(code , "403")) {
                printf("403 Forbidden\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            string resp;
            resp = strstr(response, "\r\n\r\n");
            json jResp = json::parse(resp);
            for (auto elem : jResp) {
                cout << "Title: " << elem["title"] << "; Id: " << elem["id"] << "\n";
            }
            if (jResp.size() == 0) {
                cout << "No books in library.\n\n";
                continue;
            }
            
            cout << "\n";
            continue;
        } else

        if (comm == "add_book") {
            string title, author, genre, page_count, publisher;
            printf("title:");
            getline(std::cin, title);
            printf("author:");
            getline(std::cin, author);
            printf("genre:");
            getline(std::cin, genre);
            printf("page_count:");
            getline(std::cin, page_count);
            printf("publisher:");
            getline(std::cin, publisher);

            json j;
            j["title"] = title;
            j["author"] = author;
            j["genre"] = genre;
            j["page_count"] = page_count;
            j["publisher"] = publisher;
            string data = j.dump();

            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(IP_SERVER, "/api/v1/tema/library/books",
            "application/json", data, NULL, jwt);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char cpy[1000];
            strcpy(cpy, response);
            char* code;
            code = strtok(cpy, " ");
            code = strtok(NULL, " ");

            if (!strcmp(code , "403")) {
                printf("403 Forbidden\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");

                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }
            if (!strcmp(code, "500")) {
                printf("Input must be title:<String> author:<String> genre:<String> page_count:<Number> publisher:<String>.\n\n");
                continue;
            }

            printf("Successfully added book!\n\n");
            continue;
        } else

        if (comm == "get_book") {
            string id;
            printf("id:");
            getline(std::cin, id);

            bool ok = true;
            for (auto c : id) {
                if (c == ' ') {
                    ok = false;
                    break;
                }
            }
            if (ok == false) {
                cout << "Please enter valid id\n\n";
                continue;
            }

            char url[] = "/api/v1/tema/library/books/";
            strcat(url, id.c_str());

            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(IP_SERVER, url,
            NULL, NULL, jwt);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char cpy[1000];
            strcpy(cpy, response);
            char* code;
            code = strtok(cpy, " ");
            code = strtok(NULL, " ");

            if (!strcmp(code , "404")) {
                printf("404 Not Found\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            if (!strcmp(code , "400")) {
                printf("400 Bad Request\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            if (!strcmp(code , "403")) {
                printf("403 Forbidden\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            string resp;
            resp = strstr(response, "\r\n\r\n");
            json jResp = json::parse(resp);
            for (auto elem : jResp) {
                cout << "Title: " << elem["title"] << "\n";
                cout << "Author: " << elem["author"] << "\n";
                cout << "Publisher: " << elem["publisher"] << "\n";
                cout << "Genre: " << elem["genre"] << "\n";
                cout << "Page_count: " << elem["page_count"] << "\n\n";
            }

            continue;
        } else

        if (comm == "delete_book") {
            string id;
            printf("id:");
            getline(std::cin, id);

            bool ok = true;
            for (auto c : id) {
                if (c == ' ') {
                    ok = false;
                    break;
                }
            }
            if (ok == false) {
                cout << "Please enter valid id\n\n";
                continue;
            }

            char url[] = "/api/v1/tema/library/books/";
            strcat(url, id.c_str());

            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            message = compute_delete_request(IP_SERVER, url,
            NULL, actual_cookie, jwt);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char cpy[1000];
            strcpy(cpy, response);
            char* code;
            code = strtok(cpy, " ");
            code = strtok(NULL, " ");

            if (!strcmp(code , "404")) {
                printf("404 Not Found\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            if (!strcmp(code , "400")) {
                printf("400 Bad Request\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            if (!strcmp(code , "500")) {
                printf("Input must be id:<Number>.\n\n");
                continue;
            }

            if (!strcmp(code , "403")) {
                printf("403 Forbidden\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            
            printf("Successfully deleted book!\n\n");
            continue;
        } else


        if (comm == "logout") {
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(IP_SERVER, "/api/v1/tema/auth/logout", 
            NULL, actual_cookie, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            close_connection(sockfd);

            char cpy[1000];
            strcpy(cpy, response);
            char* code;
            code = strtok(cpy, " ");
            code = strtok(NULL, " ");

            if (!strcmp(code , "400")) {
                printf("400 Bad Request\n");
                string resp;
                resp = strstr(response, "\r\n\r\n");
                json jResp = json::parse(resp);
            
                cout << jResp["error"] << "\n\n";
                continue;
            }

            memset(actual_cookie, 0, 1000);
            actual_cookie[0] = 0;
            jwt = NULL;

            printf("Successfully logged out!\n\n");
            continue;
        } else

        if (comm == "exit") {
            break;
        } else if (comm[0] != 0) {
            printf("Unrecognized command: ");
            cout << "\"" << comm << "\"" << "\n\n";
        }
    }

    return 0;
}