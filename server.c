#include "server.h"
#include "util.c"


#define DEBUG


int main(int argc, char *argv[]) 
{
	if( argc != 2) 
	{ 
		printf("%s\n", "Wrong # of arguments");
		return -1;
	} 
	signal(SIGTSTP, signal_callback_handler); // register the handler
	port = atoi(argv[1]);
	init();
	serv_conn(port);
	return 0;
}


void init() 
{ 
	// will initialize some data structures here
}


void* serv_conn (int p) 
{
    int port = p;	
  	int sd, new_sd;
  	struct sockaddr_in name, cli_name;
  	int sock_opt_val = 1;
  	int cli_len;
  	char data[80];		/* Our receive data buffer. */
  
 	if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
 	{
 		perror("(servConn): socket() error");
 	   	exit (-1);
  	}

  
  	if (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, (char *) &sock_opt_val, sizeof(sock_opt_val)) < 0) 
  	{
   		perror ("(servConn): Failed to set SO_REUSEADDR on INET socket");
    	exit (-1);
  	}

  	name.sin_family = AF_INET;
  	name.sin_port = htons (port);
  	name.sin_addr.s_addr = htonl(INADDR_ANY);
  
  	if (bind (sd, (struct sockaddr *)&name, sizeof(name)) < 0) 
  	{
   		perror ("(servConn): bind() error");
  		exit (-1);
  	}

  	listen (sd, 5);
	printf("listening\n");
  	for (;;) 
  	{
  		mysd = sd;
    	cli_len = sizeof (cli_name);
    	new_sd = accept (sd, (struct sockaddr *) &cli_name, (socklen_t* )&cli_len);
      	printf ("Assigning new socket descriptor:  %d\n", new_sd);
      
      	if (new_sd < 0) 
      	{
			perror ("(serv_conn): accept() error");
			exit (-1);
      	}       	
  //    	int n_bytes  = get_message(new_sd, buffer, sizeof(buffer));
  		if (fork() == 0)
  		{  			
    		int n_bytes = read(new_sd, buffer, sizeof(buffer));
      		printf("Message is %s \n", buffer);
      		char* token = strtok(buffer, " ");
      		token = strtok(NULL," "); 
      		strcpy(filename,token);
      		setup_document_root();
      		printf("buffer2 right now is %s\n", buffer2);
      		send_file(new_sd,++token,OK);
			printf("filename %s \n", filename);
		}
		close(new_sd);
  	}
}








void signal_callback_handler(int signum)
{
   printf("Caught signal %d\n",signum);
   // close the socket
   close(mysd);
   exit(signum);
}
