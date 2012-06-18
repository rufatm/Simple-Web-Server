#ifndef __SERVER_H
#define __SERVER_H

#define BUFFER_SIZE 1024

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

enum {TXT,HTML,JPEG,GIF,PNG,CSS,PDF,CGI};
enum {OK, NOT_FOUND}; 

#define ID 0;
#define MAXFILENAME 1024
#define TEMP_SIZE 100


#define STATUS_TEMPLATE "HTTP/1.0 %s\n"
#define HEADER_TEMPLATE "Content-Type: %s\n\n"


void init();

int get_message(int sock, char *buf, int size);
void* print_message(void*); // test function to make server simply print the message sent by the client
void* worker_function(void*);
void* serv_conn(int);
void signal_callback_handler(int signum);
//message_t* create_message(char[]);


int mysd;  // this is to hold the value of sd so that when we hit ctrl-c we still close the connection
int port;
char buffer[BUFFER_SIZE];
char filename[MAXFILENAME];
#endif
