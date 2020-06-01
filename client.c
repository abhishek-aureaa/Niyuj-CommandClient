#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <sys/errno.h>

int is_connected(int sock)
{
    unsigned char buf;
    int err = recv(sock,&buf,1,MSG_PEEK);
    return err == -1 ? 0 : 1;
}

int main(int argc, char *argv[]) {
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   char buffer[256];
   char* ls_full = NULL;
   
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
	
   portno = atoi(argv[2]);
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	
   server = gethostbyname(argv[1]);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }
   
   while(1)
   {
   printf("Please enter the message: ");
   bzero(buffer,256); //This can be larger size since the command to be sent can be bigger, check it
   fgets(buffer,255,stdin);
   
   /* Send message to the server */
   n = write(sockfd, buffer, strlen(buffer));
   //If the message sent here is "bye" , we need to close sockets and exit(0) from here as well
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   fflush(NULL);

   if(!strcmp(buffer,"bye"))
   {
      close(sockfd);
      exit(0);
   }
   
   //need to improve this code
   /* Now read server response */
   char buffer1[10000];
   bzero(buffer1,10000);
   int  is = is_connected(sockfd); //see if this is necessary

   char totalArray[20000];
   bzero(totalArray,20000);
   n = read(sockfd, buffer1, 10000);
   if (n < 0) {
    perror("ERROR reading from socket");
    exit(1); //check if there should be exit() or something other system call here
   }
   if(strcmp(buffer1, ""))
   {
      strcpy(totalArray, buffer1);	    
   }
   while(n && (!strcmp(buffer1, "")))
   {
   	bzero(buffer1,256);
   	n = read(sockfd, buffer1, 10000-1);

        if (n < 0) {
     	  perror("ERROR reading from socket");
      	  exit(1);
        }
        if(strcmp(buffer1, ""))
 	{		 
           strcpy(totalArray, buffer1);	    
	}
   }
   if(strcmp(buffer1, ""))
   {
        strcpy(totalArray, buffer1);	    
   }
   printf("%s\n",totalArray);
  }
  return 0;
}
