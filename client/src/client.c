/* File name: client.c
 *
 */

#include <stdio.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <sys/socket.h> 

#define	MAX_LIMIT	1024

int main()
{
	int sockfd; 
	char buff[MAX_LIMIT];
	struct sockaddr_in servaddr; 

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else {
		printf("Socket successfully created..\n"); 
	}
	memset(&servaddr, '\0', sizeof(servaddr)); 

	/* attach cleint to server */
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(5432);

	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else {
		printf("connected to the server..\n"); 
	}

	
	while (1) { /* run until user say bye. */
		fprintf(stdout, "Please enter a command{ls | cd <path> | pwd| bye} \n");
		scanf("%[^\n]%*c", buff); 
		(void)send(sockfd, buff, strlen(buff), 0);

		memset(buff, '\0', 1024);
		(void)recv(sockfd, buff, 1024, 0);

		/* exit if got bye from server, there can be other way*/	
		if(!strncmp(buff, "bye", 3)) {
			goto out;				
		} else {
			fprintf(stdout, "%s\n", buff);
		}
	}
out:
	close(sockfd); 
	return 0;
}
