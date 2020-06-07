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
#include <errno.h>
#include "server_impl.h"

#define MAXPENDING 	5
#define BUFFER	2048
#include <stdio.h> 

int create_socket(unsigned short port)
{
	int ret = 0;
	int opt = 1;
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

	if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
				&opt, sizeof(opt))) { 
		exit(1); 
	} 

	/* Bind to the local address */
	ret = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
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

enum cmd_type {
	CMD_LS = 1,
	CMD_CHDDIR,
	CMD_BYE,
	CMD_INVAILD, /* cmd not supported in future ther might be will have new cmd :) */
};

/* return the command type */
static int cmd_parse(char *buff)
{
	printf("cmd_parse = %s, %d\n", buff, (int)sizeof(buff));

	if((strcmp("ls", buff)) == 0)
		return CMD_LS;
	else if((strcmp("cd", buff)) == 0)
		return CMD_CHDDIR;
	else if((strcmp("bye", buff)) == 0)
		return CMD_BYE;
	else
		return CMD_INVAILD;	
} 

void handle_client(int comm_fd)
{
	int ret = 0;
	char cmd_buf[BUFFER]; /* Buffer for echo string */
	int recv_size; /* Size of received message */
	struct response res;
	int len = 0;
	/* Receive cmd from client */
	while (1) {
		printf("rev from cleint = %s\n", cmd_buf);
		recv_size = recv(comm_fd, cmd_buf, BUFFER, MSG_NOSIGNAL);
		printf("rev from cleint = %s\n", cmd_buf);

		ret = cmd_parse(cmd_buf);
		printf("ret = %d\n", ret);
		if (ret == CMD_INVAILD) {
			char *msg = "CMD NOT SUPPOTED PLEASE SEE README.md";
			res.rc = -1;
			memcpy(res.res, msg, len);
			send(comm_fd, (void *)&res, sizeof(struct response), 0);
		}
		char *ch = "hello";
		send(comm_fd, ch, 5 , 0);
		memset(cmd_buf, '\0', BUFFER);
	}
	close(comm_fd); /* Close client socket */
}
