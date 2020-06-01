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
   system("rm -rf ./bb"); //check if this is required & check if we need to change the file name
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   memset(buffer,'\0',256);
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* First call to socket() function */
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
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
   
   /* If connection is established then start communicating */
   while(1) 
   {
	   fclose(fopen("./bb", "w")); //This is required to clear up the content of the file ./bb
	   bzero(buffer,256);
	   n = read( newsockfd,buffer,255 );
	   if (n < 0) {
	      perror("ERROR reading from socket");
	      exit(1);
           }

  if((buffer[0] == 'l') && (buffer[1] == 's'))
  //if(!strstr(buffer , "ls"))
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
	  //Perhaps we can give a sleep statement here of time 10 seconds
	  exit(0);
  }
  else  if((buffer[0] == 'c') && (buffer[1] == 'd'))
  {
        printf("its change directory command\n");
        char*  temp = buffer+3; //"cd " :  space start at index 2
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
		printf("\chdir was not successful, Please check if directory is correct. %d, %d\n", errno, strlen(temp));
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
      int n = write(newsockfd,"Command not supported\n",strlen("Command not supported\n"));
      printf("Command not supported");
      system("echo Command not supported > ./bb");

       
      if (n < 0) {
         perror("ERROR writing to socket");
         exit(1);
      }
      continue;
      
  } 

  char a[400];
  memset(a,'\0',400);

  int total = 0;
  FILE* f = fopen("./bb","r");

  char* ls_full =  NULL ;
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
   ls_full = (char*) malloc(total*sizeof(char));
   memset(ls_full,'\0',total);
   memset(a,'\0',400);
   f = fopen("./bb","r");
   int slash = 0;
   while( fgets (a, 400, f)!=NULL ) {
       int i = 0;
       slash = 0;
       for(; i < strlen(a); i++)
       {
	  if(a[i]  == '/')
          {
             strcat(ls_full,"dir ");
             strcat(ls_full,a);
             slash = 1;
             break;
	  }
       }
       if(slash  == 0)
       {
         strcat(ls_full,"file ");
         strcat(ls_full,a);
       }
       else
	     slash = 0;
	// The slash code above can be modified
       memset(a,'\0',400);
    }
   }
   else  if(((buffer[0] == 'p') && (buffer[1] == 'w') && (buffer[2] == 'd')) || //PWD command
         (buffer[0] == 'c') && (buffer[1] == 'd'))
   {
       //The Full Directory name could be theoratically infinitely long, hence this loop is required
        total = 0;
        while( fgets (a, 400, f)!=NULL ) {
            total += strlen(a);
	    printf("total total : %d\n", total);
	    printf("directory name : %s\n", a);
            memset(a,'\0',400);
        }
        ls_full = (char*) malloc(total*sizeof(char));
        fclose(f); 
        memset(ls_full,'\0',total);
        memset(a,'\0',400);
        f = fopen("./bb","r");

	//We might make use of fseek here
        //fseek( f, 0, SEEK_SET );
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
   //n = write(newsockfd,ls_full,total);
   n = write(newsockfd,ls_full,strlen(ls_full));
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);  //in casse of error conditions and here as well, do I need to close the sockets
   }
   //printf("bytes written  & errno is : %d %d\n" ,n,errno);
   }
      
   return 0;
}
