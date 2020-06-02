#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/errno.h>

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   memset(buffer,'\0',256);
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* Create Socket*/
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5432;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /*Bind the socket to port.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /*Limit the connections here*/
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   /* Accept the actual connections here*/
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
   
   /* If connection is established then start communicating */
   while(1) 
   {
   fclose(fopen("./bb", "w")); //Clear up file content for next round of command
   bzero(buffer,256);
   n = read( newsockfd,buffer,255 );
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

  if((buffer[0] == 'l') && (buffer[1] == 's'))
  {
        printf("its LS command\n"); 
  	system("ls -1F > ./bb");
  }
  else  if((buffer[0] == 'p') && (buffer[1] == 'w') && (buffer[2] == 'd'))
  {
        printf("its PWD command\n");
  	system("pwd > ./bb");
  }
  else  if((buffer[0] == 'b') && (buffer[1] == 'y') && (buffer[2] == 'e'))
  {
	  close(sockfd);
	  close(newsockfd);
	  exit(0);
  }
  else  if((buffer[0] == 'c') && (buffer[1] == 'd'))
  {
        printf("its change directory command\n");
        char*  temp = buffer+2; /*"cd " */
	printf("directory name :%s\n", temp);
	char str[400];
	memset(str,'\0', 400);

	while((*temp) == ' ')
	{ 
		temp++;
	}
	strncpy(str, temp, strlen(temp)-1);
	if (chdir(str) != 0)
	{
		printf("chdir was not successful, Perhaps directory does not exist.");
		system("echo Incorrect Directory > ./bb");
	}
	else
	{
		printf("chdir was successful\n");
	  	system("pwd > ./bb");
	}
  }
  else
  {
      char* message = "Command not supported\n";
      int n = write(newsockfd,message,strlen(message));
      printf("Command not supported");
      system("echo Command not supported > ./bb");

       
      if (n < 0) {
         perror("ERROR writing to socket");
         exit(1);
      }
      continue;  //proceed with handling of next command
  } 

  char a[400];
  memset(a,'\0',400);

  int total = 0;
  FILE* f = fopen("./bb","r");

  char* ls_full =  NULL ;

  /*Calculate length of the result of the command executed here*/
  if((buffer[0] == 'l') && (buffer[1] == 's')) { //LS command 
   	while( fgets (a, 400, f)!=NULL ) {
	int i = 0;
	for(; i < strlen(a); i++)
	{
		if(a[i]  == '/')
       		{
		    break;
		}
   	}
	if(a[i]  == '/')
   	{
           printf("slash found : %s \n" , a);
           total += 4 + sizeof(a); //"dir "
      	}
        else {
           total += 5 + sizeof(a); //"file "
        }
        memset(a,'\0',400);
      }
   fclose(f);

   /*read the command result, Create message for it & Send/Write*/
   ls_full = (char*) malloc(total*sizeof(char));
   memset(ls_full,'\0',total);
   memset(a,'\0',400);
   f = fopen("./bb","r");
   int slash = 0;
   while( fgets (a, 400, f)!=NULL ) {
       int i = 0;
       slash = 0;


       char creationDate[500]; //"ls command + output dir + filename: 500 should be enough
       char dateTime[20];
       memset(creationDate,'\0',500);
       memset(dateTime,'\0',20);
       strcat(creationDate, "ls -l --time-style=+%F  > ./output ");
       strcat(creationDate, a);
       system(creationDate);
       system("awk '{print $6}' ./output > ./dateStamp");
       FILE* ff = fopen("./dateStamp", "r");
       fgets (dateTime, 20, ff);
       printf(dateTime);
       fclose(ff);
       

       /*before handle "file" or "dir" prefixes */
       for(; i < strlen(a); i++)
       {
	  if(a[i]  == '/')
          {
             strcat(ls_full,"dir ");
             slash = 1;
             break;
	  }
       }
       if(slash  == 0)
       {
         strcat(ls_full,"file ");
       }
       else
	     slash = 0;

       strcat(ls_full,a);
       strcat(ls_full,dateTime);
       strcat(ls_full,"\n");

       memset(a,'\0',400);
    }
   }
   else  if(((buffer[0] == 'p') && (buffer[1] == 'w') && (buffer[2] == 'd')) || //PWD command
         (buffer[0] == 'c') && (buffer[1] == 'd'))
   {
       //The Full Directory name could be theoratically infinitely long, hence this loop is required
        total = 0;
        /*Calculate length of the result of the command executed here*/
        while( fgets (a, 400, f)!=NULL ) {
            total += strlen(a);
	    printf("total total : %d\n", total);
	    printf("directory name : %s\n", a);
            memset(a,'\0',400);
        }
        ls_full = (char*) malloc(total*sizeof(char));
        memset(ls_full,'\0',total);
        memset(a,'\0',400);
        fseek( f, 0, SEEK_SET );
	
        /*read the command result, Create message for it & Send/Write*/
        while( fgets (a, 400, f)!=NULL ) {
            strcat(ls_full,a);
            memset(a,'\0',400);
        }
        fclose(f); 
   }
   else  {
	printf("This Command is not supported\n");
   }

   /* Write a response to the client */
   n = write(newsockfd,ls_full,strlen(ls_full));
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);  //in casse of error conditions and here as well, do I need to close the sockets
   }
   }
      
   return 0;
}
