/*File Name: server_impl.c
 *
 *
 */
#include <stdio.h> 
#include <sys/socket.h> /* for socket(), bind(), connect(), recv() and send() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include "common.h"
#include "log.h"
#include <errno.h>
#include "server_impl.h"

#define MAXPENDING 	5

#define BUFFER	2048

int create_socket(unsigned short port)
{
	int ret = 0;
	int opt = 1;
	int sock_fd; /* Socket descriptor for server */
	int comm_fd; /* Socket descriptor for client */
	struct sockaddr_in server_addr; /* Local address */

	log_debug("Entry: %s", __func__);
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

	log_debug("EXIT: %s", __func__);
	return sock_fd;
}

int accept_connection(int sock_fd)
{
	log_debug("Entry: %s", __func__);
	struct sockaddr_in client_addr; /* Client address */
	unsigned int client_len; /* Length of client address data structure */

	int comm_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);	
	assert(comm_fd > 0);

	log_debug("EXIT: %s", __func__);
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
	int rc = -1;

	log_debug("Entry: %s buff = %s\n", __func__, buff);

	if((strcmp("ls", buff)) == 0)
		rc =  CMD_LS;
	else if((strcmp("cd", buff)) == 0)
		rc = CMD_CHDDIR;
	else if((strcmp("bye", buff)) == 0)
		rc = CMD_BYE;
	else
		rc = CMD_INVAILD;	

	log_debug("EXIT: %s", __func__);
	return rc;
} 

static void find_type(char *buff, int type)
{
	log_debug("Entry: %s", __func__);
	switch (type) {
		case DT_UNKNOWN:
			strncpy(buff, "UNKNOWN", 7);
			break;
		case DT_FIFO:
			strncpy(buff, "FILO", 4);
			break;
		case DT_DIR:
			strncpy(buff, "DIR", 3);
			break;
		case DT_CHR:
			strncpy(buff, "CHR", 3);
			break;
		case DT_BLK:
			strncpy(buff, "BLK", 3);
			break;
		case DT_REG:
			strncpy(buff, "REG", 3);
			break;
		case DT_LNK:
			strncpy(buff, "LNK", 3);
			break;
		case DT_SOCK:
			strncpy(buff, "SOCK", 4);
			break;
		case DT_WHT:
			strncpy(buff, "WHT", 3);
			break;
		default:

			break;

	}
	log_debug("EXIT: %s", __func__);
}

static void process_ls()
{
	DIR *d;
	struct dirent *dir;
	char type[10];
	struct stat st;
	
	log_debug("Entry: %s", __func__);

	memset(type, '\0', 10);
	d = opendir(".");

	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if(!strncmp(dir->d_name, ".", 2) || !strncmp(dir->d_name, "..", 3)) {
				printf("here\n");
				continue; /* do want to expose whole file system to cleint */
			}
			find_type(type, dir->d_type);
			int ret = stat(dir->d_name, &st);
			log_debug("entry:<%s>:type<%s>:data<%s>", dir->d_name, type, ctime(&st.st_ctime));
			memset(type, '\0', 10);

		}
		closedir(d);
	}

	log_debug("EXIT:%s", __func__);
}

void handle_client(int comm_fd)
{
	int ret = 0;
	char cmd_buf[BUFFER]; /* Buffer for echo string */
	int recv_size; /* Size of received message */
	struct response res;
	int len = 0;

	log_debug("Entry:%s", __func__);
	/* Receive cmd from client */
	while (1) {
		recv_size = recv(comm_fd, cmd_buf, BUFFER, MSG_NOSIGNAL);
		ret = cmd_parse(cmd_buf);
		log_debug("ret:%d", ret); 
		switch (ret){
			case CMD_LS:
				process_ls();	
				break;
			case CMD_CHDDIR:
				break;
			case CMD_BYE:
				goto out;
				break;

			default:
				log_error("Something bad happen");
				break;
		}
		if (ret == CMD_INVAILD) {
			log_info("command not supported\n");
			char *msg = "CMD NOT SUPPOTED PLEASE SEE README.md";
			res.rc = -1;
			memcpy(res.res, msg, len);
			send(comm_fd, (void *)&res, sizeof(struct response), 0);
		}

		char *ch = "hello";
		send(comm_fd, ch, 5 , 0);
		memset(cmd_buf, '\0', BUFFER);
	}
out:
	close(comm_fd); /* Close client socket */
	log_debug("EXIT: %s", __func__);
}
