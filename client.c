#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"
#include "define.h"
#include "parson.h"

/* Extrage mesajul de eroare din raspunsul serverului */
void print_error(char *response)
{
    char *error, aux[SMALL_SIZE];
    error = strstr(response, ERROR);
                    
    error = strtok(error, ":");
    error = strtok(NULL, ":");
    error = strtok(error, "}");

    strcpy(aux, error + 1);
    aux[strlen(aux) - 1] = '\0';
    printf("%s\n", aux);
}
/* Verifica daca id-ul introdus este un numar */
int is_id_int(char *id)
{
    for(int i = 0; i < strlen(id); i++)
    {
        if(isdigit(id[i])== 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int sockfd;
    char command[SMALL_SIZE], book_url[LINELEN], delete_url[LINELEN], id[SMALL_SIZE], aux[LINELEN];
    char *response, *get_req, *post_req, *delete_req, *user_data, *book_data;
    char *success, *cookie, *cookie_aux, *too_many_requests, *jwt, *token;
    cookie = jwt = token = NULL;

    while(1)
    {
        /* citeste comanda */
        scanf("%s", command);
        /* deschide conexiunea */
        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

        if(sockfd < 0)
        {
            printf("Openning connection ERROR.\n");
            return 0;
        }

        if(strcmp(command, REGISTER) == 0)
        {
            user_data = get_user_json_format_string();
            post_req = compute_post_request(HOST, REGISTER_URL, CONTENT_TYPE, &user_data, 1, NULL, 0, NULL);
            send_to_server(sockfd, post_req);
            response = receive_from_server(sockfd);
            
            success = strstr(response, CREATED_201);
            too_many_requests = strstr(response, SERVER_ERROR);
            
            /* daca apare eroare sau sunt prea multe cereri */
            if(success == NULL)
                if(too_many_requests == NULL)
                    print_error(response);
                else
                    printf("Too many requests.\n");
            else 
                printf("User registered successfully.\n");   
        }
        else if(strcmp(command, LOGIN) == 0)
        {
            user_data = get_user_json_format_string();
            post_req = compute_post_request(HOST, LOG_URL, CONTENT_TYPE, &user_data, 1, NULL, 0, NULL);
            send_to_server(sockfd, post_req);
            response = receive_from_server(sockfd);

            success = strstr(response, OK_200);
            too_many_requests = strstr(response, SERVER_ERROR);

            /* daca apare eroare sau sunt prea multe cereri */
            if(success == NULL)
                if(too_many_requests == NULL)
                    print_error(response);
                else
                    printf("Too many requests.\n");
            else   
            {   
                if(cookie != NULL)
                    printf("You are already loggined.\n");
                else
                {
                    /* extrag cookie-ul si afisez un mesaj de succes la logare */
                    cookie = calloc(SMALL_SIZE, sizeof(char));
                    cookie_aux = strstr(response, "connect.sid");
                    cookie_aux = strtok(cookie_aux, ";");
                    strcpy(cookie, cookie_aux);
                    printf("You are now loggined.\n");
                }
            }
        }
        else if(strcmp(command, ENTER_LIBRARY) == 0)
        {
            if(cookie == NULL)
            {
                printf("You are not loggined.\n");
                continue;
            }

            get_req = compute_get_request(HOST, ACCESS_URL, NULL, &cookie, 1, NULL);
            send_to_server(sockfd, get_req);
            response = receive_from_server(sockfd);
          
            success = strstr(response, OK_200);
            too_many_requests = strstr(response, SERVER_ERROR);

            /* daca apare eroare sau sunt prea multe cereri */
            if(success == NULL)
                if(too_many_requests == NULL)
                    print_error(response);
                else
                    printf("Too many requests.\n");
            else
            { 
                /* extrag JWT si afisez un mesaj de succes la intrarea in biblioteca */
                token = strstr(response, "token");
                token = strtok(token, ":");
                token = strtok(NULL, ":");
                token = strtok(token, "}");
                strcpy(aux, token + 1);
                token = NULL;
                aux[strlen(aux) - 1] = '\0';
                jwt = calloc(LINELEN, sizeof(char));
                strcpy(jwt, aux);
                printf("You have entered the library.\n");
            }
        }
        else if(strcmp(command, GET_BOOKS) == 0)
        {
            if(cookie == NULL)
            {
                printf("You are not loggined.\n");
                continue;
            }
            if(jwt == NULL)
            {
                printf("Acces denied.\n");
                continue;
            }

            get_req = compute_get_request(HOST, BOOKS_URL, NULL, &cookie, 1, jwt);
            send_to_server(sockfd, get_req);
            response = receive_from_server(sockfd);
        
            success = strstr(response, OK_200);
            too_many_requests = strstr(response, SERVER_ERROR);

            /* daca apare eroare sau sunt prea multe cereri */
            if(success == NULL)
                if(too_many_requests == NULL)
                    print_error(response);
                else
                    printf("Too many requests.\n");
            else
                printf("Books:\n%s\n", strstr(response, "["));
    
        }
        else if(strcmp(command, GET_BOOK) == 0)
        {
            if(cookie == NULL)
            {
                printf("You are not loggined.\n");
                continue;
            }
            if(jwt == NULL)
            {
                printf("Acces denied.\n");
                continue;
            }

            printf("id=");
			scanf("%s", id);
            while(is_id_int(id) == 0)
            {
                printf("Wrong format, enter a number.\n");
                printf("id=");
			    scanf("%s", id);
            }
            /*construiesc url-ul cartii */
            sprintf(book_url, "%s/%s", BOOKS_URL, id);
            
            get_req = compute_get_request(HOST, book_url, NULL, &cookie, 1, jwt);
            send_to_server(sockfd, get_req);
            response = receive_from_server(sockfd);
           
            success = strstr(response, OK_200);
            too_many_requests = strstr(response, SERVER_ERROR);

            /* daca apare eroare sau sunt prea multe cereri */
            if(success == NULL)
                if(too_many_requests == NULL)
                    print_error(response);
                else
                    printf("Too many requests.\n");
            else
                printf("Book's info:\n%s\n", strstr(response, "["));
        }
        else if(strcmp(command, ADD_BOOK) == 0)
        {
            if(cookie == NULL)
            {
                printf("You are not loggined.\n");
                continue;
            }
            if(jwt == NULL)
            {
                printf("Acces denied.\n");
                continue;
            }
            book_data = get_book_json_format_string();
            post_req = compute_post_request(HOST, BOOKS_URL, CONTENT_TYPE, &book_data, 1, NULL, 0, jwt);
            send_to_server(sockfd, post_req);
            response = receive_from_server(sockfd);
            
            success = strstr(response, OK_200);
            too_many_requests = strstr(response, SERVER_ERROR);

            /* daca apare eroare sau sunt prea multe cereri */
            if(success == NULL)
                if(too_many_requests == NULL)
                    print_error(response);
                else
                    printf("Too many requests.\n");
            else
                printf("Book has been added.\n");
        }
        else if(strcmp(command, DELETE_BOOK) == 0)
        {
            if(cookie == NULL)
            {
                printf("You are not loggined.\n");
                continue;
            }
            if(jwt == NULL)
            {
                printf("Acces denied.\n");
                continue;
            }
            
            printf("id: ");
			scanf("%s", id);

            while(is_id_int(id) == 0)
            {
                printf("Wrong format, enter a number.\n");
                printf("id=");
			    scanf("%s", id);
            }

            sprintf(delete_url, "%s/%s", BOOKS_URL, id);

            delete_req = compute_delete_request(HOST, delete_url, NULL, &cookie, 1, jwt);
            send_to_server(sockfd, delete_req);
            response = receive_from_server(sockfd);
           
           /* daca apare eroare sau sunt prea multe cereri */
            success = strstr(response, OK_200);
            too_many_requests = strstr(response, SERVER_ERROR);

            if(success == NULL)
                if(too_many_requests == NULL)
                    print_error(response);
                else
                    printf("Too many requests.\n");
            else
                printf("Book has been deleted.\n");
        }
        else if(strcmp(command, LOGOUT) == 0)
        {
            get_req = compute_get_request(HOST, LOGOUT_URL, NULL, &cookie, 1, NULL);
            send_to_server(sockfd, get_req);
            response = receive_from_server(sockfd);

            success = strstr(response, OK_200);
            too_many_requests = strstr(response, SERVER_ERROR);

            /* daca apare eroare sau sunt prea multe cereri */
            if(success == NULL)
                if(too_many_requests == NULL)
                    print_error(response);
                else
                    printf("Too many requests.\n");
            else
            {
                free(cookie);
                cookie = NULL;
                free(jwt);
                jwt = NULL;
                printf("User has logout.\n");
            }
        }
        else if(strcmp(command, EXIT) == 0)
        {
            break;
        }
        else
        {
            printf("Unknown command.\n");
        }
        
        close(sockfd);

    }
    return 0;
}
