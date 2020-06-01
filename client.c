#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   char buffer[256];
   char* ls_full = NULL;
   
   if (argc < 3) {
      fprintf(stderr,"%s hostname port\n", argv[0]);
      exit(0);
   }
	
   portno = atoi(argv[2]);
   
   /* Create socket */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("sockert() failed");
      exit(1);
   }
	
   server = gethostbyname(argv[1]);
   if (server == NULL) {
      fprintf(stderr,"Could not retrieve hostname\n");
      exit(1);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("connect() failed");
      exit(1);
   }
   
   printf("Client Side Command Prompt, Please enter commands\n: ");
   while(1)
   {
   printf("Command : ");
   bzero(buffer,256); 
   fgets(buffer,255,stdin);
   
   /* Send message to the server */
   n = write(sockfd, buffer, strlen(buffer));
   
   if (n < 0) {
      perror("write() failed\n");
      exit(1);
   }
   //fflush(NULL); //optional but useful, in our context

   //"bye" message
   if((buffer[0] == 'b') && (buffer[1] == 'y') && (buffer[2] == 'e'))
   {
      close(sockfd);
      exit(0);
   }
   
   /* Now read server response */
   char buffer1[1000];
   bzero(buffer1,1000);

   char totalArray[2000];
   bzero(totalArray,2000);
   n = read(sockfd, buffer1, 1000);
   if (n < 0) {
    perror("ERROR reading from socket");
    exit(1); //check if there should be exit() or something other system call here
   }
   //if(strcmp(buffer1, ""))
   {
      strcpy(totalArray, buffer1);	    
   }
   while(n && (!strcmp(buffer1, "")))
   {
   	bzero(buffer1,256);
   	n = read(sockfd, buffer1, 1000); //1000-1);

        if (n < 0) {
     	  perror("ERROR reading from socket");
      	  exit(1);
        }
        //if(strcmp(buffer1, ""))
 	{		 
           strcpy(totalArray, buffer1);	    
	}
   }
   //if(strcmp(buffer1, ""))
   {
        strcpy(totalArray, buffer1);	    
   }
   printf("%s\n",totalArray);
  }
  return 0;
}
