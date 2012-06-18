#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include "server.h"


static char *p;
static char buffer2[MAXFILENAME];
static char temp[TEMP_SIZE];


void setup_document_root() {
 getcwd(buffer2,MAXFILENAME);
 p = buffer2+strlen(buffer2);
 strcpy(p,"/public_html/");
 p+=strlen("/public_html/");
 return;
}



char* get_direc(char *source) {
 int len = strlen(source);
 char *s = source+len;
 while(s >= source && *s != '/') {
   s--;
 }
 *s = 0;
 return s;
}



int filetype_of_filename(char *filename) {
 int len = strlen(filename);
 char *c = filename+len;
 while(c >= filename && *c != '.') {
   c--;
 }
 c++;

 if(strcmp(c,"html") == 0) {
   return HTML;
 } else if (strcmp(c,"jpg") == 0) {
   return JPEG;
 } else if (strcmp(c,"jpeg") == 0) {
   return JPEG;
 } else if (strcmp(c,"png") == 0) {
   return PNG;
 } else if (strcmp(c,"gif") == 0) {
   return GIF;
 } else if (strcmp(c,"css") == 0) {
   return CSS;
 } else if (strcmp(c,"pdf") == 0) {
   return PDF;
 } else if (strcmp(c,"cgi") == 0) { 
 	return CGI; }
 return TXT;
}






int send_status(int client_fd, int status) {
 char* status_text;
 if (status == OK)
 	status_text = "200 OK";
 else 
 	status_text = "400 NOT FOUND";
 snprintf(temp,TEMP_SIZE,STATUS_TEMPLATE,status_text);
 Writen(client_fd,temp,strlen(temp));
 return 0;
}




int send_headers(int client_fd, int type) {
 char *content;
 switch(type) {
 default:
 case TXT:
   content = "text/plain";
   break;
 case HTML:
   content = "text/html";
   break;
 case JPEG:
   content = "image/jpg";
   break;
 case GIF:
   content = "image/png";
   break;
 case CSS:
   content = "text/css";
   break;
 case PDF:
   content = "application/pdf";
   break;
 }
 snprintf(temp,TEMP_SIZE,HEADER_TEMPLATE,content);
 Writen(client_fd,temp,strlen(temp));
 return 0;
}



int execute(int client_fd, char *filename) {
 int fd;
 strncpy(p,filename,MAXFILENAME-strlen(buffer2)-1);
 char *args[2] = {buffer2,NULL};
 char *tmp;

 if( (fd = open(buffer2, O_RDONLY)) < 0 ) {
   send_status(client_fd, NOT_FOUND);
   send_file(client_fd,"404.html",1);
   return;
 }
 tmp = get_direc(buffer2);
 chdir(buffer2);
 *tmp = '/';
 send_status(client_fd,OK);
 dup2(client_fd,STDOUT_FILENO);
 close(client_fd);
 execvp(args[0],args);
 return 0;
}






int send_file(int client_fd, char *filename, int statsent) {
 printf("in\n");
 int type;
 int pid;
 char *addr;
 struct stat sb;
 int fd;
  int t = filetype_of_filename(filename);
  strncpy(p,filename,MAXFILENAME - strlen(buffer2)-1);

  printf("buffer2!!!!! %s \n", buffer2 );

 if( (fd = open(buffer2, O_RDONLY)) < 0) {
   *p = '\0';
   send_status(client_fd,NOT_FOUND);
   send_file(client_fd,"404.html", 1);
   return -1;
 }


// char *args[2] = {"ls",  NULL};


 if( fstat(fd,&sb) < 0) {
   fprintf(stderr,"Failed to stat %s\n",buffer2);
   exit(1);
 }

 if(S_ISDIR(sb.st_mode)) 
 { 
  char *args[3] = {"ls",buffer2,NULL};
   send_status(client_fd,OK);
   printf("sent status \n");
   send_headers(client_fd,TXT);
   printf("sent headers %d \n", client_fd);
   dup2(client_fd,STDOUT_FILENO);
   fprintf(stderr,"redirected %d\n", client_fd);
   close(client_fd);
   fprintf(stderr,"closed  %d\n", client_fd);
	if(execvp(args[0],args)) {
    	 fprintf(stderr,"Internal Error %s\n",buffer2);
    	 exit(1);
   }
 }
 else 
 {
 	if (t == CGI) 
 	{ 
		
 	 	char* args[] = {buffer2, buffer2, NULL};
 	 	//char* args[] = {"./test.cgi",  NULL};

		dup2(client_fd, STDOUT_FILENO);
		close(client_fd); 
		execvp(args[0],args);
 	}
 	else 
 	{
 	char* args[] = {"cat", buffer2, NULL};
	send_status(client_fd, OK);
	send_headers(client_fd, t); 
	dup2(client_fd, STDOUT_FILENO);
	close(client_fd); 
	execvp(args[0],args);
	}
}	


   
 

     fprintf(stderr,"about to map\n");


 addr = mmap(NULL,sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

 if(addr == MAP_FAILED) {
   fprintf(stderr,"Failed to map file %s\n",buffer2);
   exit(1);
 }

 if(!statsent) {
   send_status(client_fd,OK);
 }

 send_headers(client_fd,filetype_of_filename(filename));
 Writen(client_fd,addr,sb.st_size);

 return 0;
}







ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
	{
		//err_sys("writen error");
		}
}


