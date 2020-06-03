/*File Name: server_impl.c
 *
 *
 */
#include <sys/socket.h> /* for socket(), bind(), connect(), recv() and send() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include "common.h"
#include "server_impl.h"

#define MAXPENDING 	5
#define RCVBUFSIZE	2048
#include <stdio.h> 

int create_socket(unsigned short port)
{
	int ret = 0;
	int sock_fd; /* Socket descriptor for server */
	int comm_fd; /* Socket descriptor for client */
	struct sockaddr_in server_addr; /* Local address */

	/* Create socket for incoming connections */
	sock_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(sock_fd != 0);

	/* Construct local address structure */
	memset(&server_addr, 0, sizeof(server_addr)); /* Zero out structure */
	server_addr.sin_family = AF_INET; /* Internet address family */
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	server_addr.sin_port = htons(port); /* port */

	/* Bind to the local address */
	ret = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	printf("rc = %d\n", ret);
	assert(ret == 0);

	/* Mark the socket so it will listen for incoming connections */
	ret = listen(sock_fd, MAXPENDING);
	assert(ret == 0);

	return sock_fd;
}

int accept_connection(int sock_fd)
{
	struct sockaddr_in client_addr; /* Client address */
	unsigned int client_len; /* Length of client address data structure */

	int comm_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);	
	assert(comm_fd > 0);

	return comm_fd;
}

void handle_client(int comm_fd)
{
	int ret = 0;
	char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
	int recvMsgSize; /* Size of received message */

	/* Receive message from client */
	recvMsgSize = recv(comm_fd, echoBuffer, RCVBUFSIZE, 0);
	assert(recvMsgSize > 0);
	
	/* Send received string and receive again until end of transmission */
	while (recvMsgSize > 0) /* zero indicates end of transmission */
	{
		/* Echo message back to client */
		ret = send(comm_fd, echoBuffer, recvMsgSize, 0);
		assert(ret == recvMsgSize);
		
		/* See if there is more data to receive */
		recvMsgSize = recv(comm_fd, echoBuffer, RCVBUFSIZE, 0);
		//assert(recvMsgSize > 0);
	}

	close(comm_fd); /* Close client socket */
}
