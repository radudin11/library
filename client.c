#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

char* host = "34.254.242.81";
char* urlLogin = "/api/v1/tema/auth/login";
char* urlRegister = "/api/v1/tema/auth/register";
char* urlLibrary = "/api/v1/tema/library/access";
char* urlBooks = "/api/v1/tema/library/books";
char* urlLogout = "/api/v1/tema/auth/logout";

char* cookie = NULL;
char* token = NULL;


void registerUser(int sockfd) {
    char* username = calloc(100, sizeof(char));
    char* password = calloc(100, sizeof(char));
    
    char* serializedString;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);



    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    serializedString = json_serialize_to_string_pretty(root_value);

    char *message = compute_post_request(host, urlRegister, "application/json", &serializedString, 1, NULL, 0, NULL);
    printf("%s\n", message);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    printf("%s\n", response);
    json_free_serialized_string(serializedString);
    json_value_free(root_value);

    if (strstr(response, "error") != NULL) {
        printf("Error register\n");
    } else {
        printf("Register successful\n");
    }

    free(message);
    free(response);
    free(username);
    free(password);

}

void login(int sockfd) {
    char* username = calloc(100, sizeof(char));
    char* password = calloc(100, sizeof(char));
    
    char* serializedString;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    serializedString = json_serialize_to_string_pretty(root_value);

    char *message = compute_post_request(host, urlLogin, "application/json", &serializedString, 1, NULL, 0, NULL);
    printf("%s\n", message);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    printf("%s\n", response);

    char* aux = strstr(response, "Set-Cookie: ");
    if (aux != NULL) {
        aux += strlen("Set-Cookie: ");
        cookie = calloc(100, sizeof(char));
        sscanf(aux, "%s", cookie);
        printf("Login successful\n");
    } else {
        printf("Error login\n");
    }

    free(message);
    free(response);
    free(username);
    free(password);
    json_free_serialized_string(serializedString);
    json_value_free(root_value);
}

void enter_library(int sockfd) {
    JSON_Value *root_value;

   char* message = NULL;
    if (cookie != NULL )
        message = compute_get_request(host, urlLibrary, NULL, &cookie, 1, NULL);
    else
        message = compute_get_request(host, urlLibrary, NULL, NULL, 1, NULL);
    printf("%s\n", message);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    printf("%s\n", response);

    if (cookie == NULL) {
        printf("ERROR: Not logged in\n");
        free(message);
        free(response);
        return;
    }

    if (strstr(response, "error") != NULL) {
        printf("Error\n");
        free(message);
        free(response);
        return;
    }

    char* aux = strstr(response, "{");
    if (aux == NULL) {
        printf("Error\n");
        free(message);
        free(response);
        return;
    }

    if (*(aux + 2) != 't'){
        printf("Error\n");
        free(message);
        free(response);
        return;
    }

    root_value = json_parse_string(aux);
    if (json_value_get_type(root_value) != JSONObject) {
        free(message);
        free(response);
        json_value_free(root_value);
        printf("Error\n");
        return;
    }
    JSON_Object *root_object = json_value_get_object(root_value);
    token = calloc(1000, sizeof(char));
    strcpy(token, json_object_get_string(root_object, "token"));

    printf("Enter library successful\n");



    free(message);
    free(response);
    json_value_free(root_value);
}

void get_books(int sockfd) {

    char* message = NULL;
    if (cookie != NULL )
        message = compute_get_request(host, urlBooks, NULL, &cookie, 1, token);
    else
        message = compute_get_request(host, urlBooks, NULL, NULL, 1, NULL);

    printf("%s\n", message);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    printf("%s\n", response);

    if (cookie == NULL)
        printf("ERROR: Not logged in\n");

    if (token == NULL)
        printf("ERROR: Not in library\n");

    if (strstr(response, "error") != NULL) {
        printf("Error\n");
    }

    if (strstr(response, "[]") != NULL) {
        printf("No books added\n");
    }

    free(message);
    free(response);
}

void get_book(int sockfd) {
    int id;
    printf("id=");
    scanf("%d", &id);

    char *url = calloc(1000, sizeof(char));
    sprintf(url, "%s/%d", urlBooks, id);

    char* message = NULL;
    if (cookie != NULL )
        message = compute_get_request(host, url, NULL, &cookie, 1, token);
    else
        message = compute_get_request(host, url, NULL, NULL, 1, token);
    
    printf("%s\n", message);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    printf("%s\n", response);

    if (cookie == NULL)
        printf("ERROR: Not logged in\n");

    if (token == NULL)
        printf("ERROR: Not in library\n");

    free(message);
    free(response);
    free(url);
}

void add_book(int sockfd) {
    char* title = calloc(100, sizeof(char));
    char* author = calloc(100, sizeof(char));
    char* genre = calloc(100, sizeof(char));
    char* publisher = calloc(100, sizeof(char));
    int page_count = 0;

    char* serializedString;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);



    getc(stdin);
    size_t len = 0;
    printf("title=");
    getline(&title, &len, stdin);
    title[strlen(title) - 1] = '\0';

    printf("author=");
    getline(&author, &len, stdin);
    author[strlen(author) - 1] = '\0';

    printf("genre=");
    getline(&genre, &len, stdin);
    genre[strlen(genre) - 1] = '\0';

    printf("publisher=");
    getline(&publisher, &len, stdin);
    publisher[strlen(publisher) - 1] = '\0';

    printf("page_count=");
    scanf("%d", &page_count);

    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "publisher", publisher);
    json_object_set_number(root_object, "page_count", page_count);

    serializedString = json_serialize_to_string_pretty(root_value);

    char* message = NULL;

    if (cookie != NULL)
        message = compute_post_request(host, urlBooks, "application/json", &serializedString, 1, &cookie, 1, token);
    else
        message = compute_post_request(host, urlBooks, "application/json", &serializedString, 1, NULL, 1, token);
    printf("%s\n", message);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    printf("%s\n", response);

    if (cookie == NULL)
        printf("ERROR: Not logged in\n");

    if (token == NULL)
        printf("ERROR: Not in library\n");
        
    if (strstr(response, "error") != NULL) {
        printf("Error\n");
        free(message);
        free(response);
        free(title);
        free(author);
        free(genre);
        free(publisher);
        json_free_serialized_string(serializedString);
        json_value_free(root_value);
        return;
    }

    printf("Book added successfully:\n");
    printf("%s\n", serializedString);

    free(message);
    free(response);
    free(title);
    free(author);
    free(genre);
    free(publisher);
    json_free_serialized_string(serializedString);
    json_value_free(root_value);

}

void delete_book(int sockfd) {
    int id;
    printf("id=");
    scanf("%d", &id);

    char *url = calloc(1000, sizeof(char));
    sprintf(url, "%s/%d", urlBooks, id);

    char* message= NULL;
    if (cookie != NULL)
        message = compute_delete_request(host, url, NULL, &cookie, 1, token);
    else
        message = compute_delete_request(host, url, NULL, NULL, 1, token);
    printf("%s\n", message);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    printf("%s\n", response);

    if (cookie == NULL)
        printf("ERROR: Not logged in\n");

    if (token == NULL)
        printf("ERROR: Not in library\n");

    if (strstr(response, "error") != NULL) {
        printf("Error\n");
        free(message);
        free(response);
        free(url);
        return;
    }

    printf("Book deleted successfully\n");

    free(message);
    free(response);
    free(url);
}


void logout(int sockfd) {
    
    char* message = NULL;
    if (cookie != NULL )
        message = compute_get_request(host, urlLogout, NULL, &cookie, 1, token);
    else
        message = compute_get_request(host, urlLogout, NULL, NULL, 1, NULL);
    printf("%s\n", message);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    printf("%s\n", response);

    if (strstr(response, "error") != NULL) {
        printf("Error\n");
        free(message);
        free(response);
        return;
    }

    printf("Logged out successfully\n");

    free(message);
    if (cookie != NULL)
        free(cookie);
    if (token != NULL)
        free(token);

    token = NULL;
    cookie = NULL;

    free(response);
}




int main(int argc, char *argv[])
{
    int sockfd;

    setvbuf(stdin, NULL, _IONBF, BUFSIZ);
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    setvbuf(stderr, NULL, _IONBF, BUFSIZ);




    char* command = calloc(100, sizeof(char));
    
    while(1) {
        scanf("%s", command);
        sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);
        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "register") == 0) {
            registerUser(sockfd);
        } else if (strcmp(command, "login") == 0) {
            login(sockfd);
        } else if (strcmp(command, "enter_library") == 0) {
            enter_library(sockfd);
        } else if (strcmp(command, "get_books") == 0) {
            get_books(sockfd);
        } else if (strcmp(command, "get_book") == 0) {
            get_book(sockfd);
        } else if (strcmp(command, "add_book") == 0) {
            add_book(sockfd);
        } else if (strcmp(command, "delete_book") == 0) {
            delete_book(sockfd);
        } else if (strcmp(command, "logout") == 0) {
            logout(sockfd);
        } else {
            printf("Invalid command\n");
        }


    }

    return 0;
}
