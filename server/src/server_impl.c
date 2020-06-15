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
#include <sys/stat.h> /* for stat */
#include <time.h> /* for ctime */
#include <dirent.h> /* for readdir */
#include <errno.h>/* for errno */
#include <assert.h> /* for assert */
#include "log.h" /* logging */
#include "server_impl.h" /* for server helper function */

#define MAXPENDING 	5
#define BUFFER	2048

/* enumaratino for cmd type
 *
 */
enum cmd_type {
	CMD_LS = 1, /* list */
	CMD_CHDDIR, /* change dir */
	CMD_BYE,   /* terminate */
	CMD_INVAILD, /* cmd not supportedi, in future ther might be will have new cmd :) */
};

/* return the command type 
 *
 * @param[in]: cmd
 * return 0 on success otherwise -1 or CMD_INVAILD
 */
static int cmd_parse(char *cmd)
{
	int rc = -1;

	log_debug("Entry:%s", __func__);

	if ((strcmp("ls", cmd)) == 0) {
		rc =  CMD_LS;
	} else if ((strcmp("cd", cmd)) == 0) {
		rc = CMD_CHDDIR;
	} else if ((strcmp("bye", cmd)) == 0) {
		rc = CMD_BYE;
	} else {
		rc = CMD_INVAILD;	
	}

	log_debug("EXIT:%s", __func__);
	return rc;
} 

/* find type for each enrty found.
 *
 * @param[out]: buff 
 * @param[in]: type 
 *
 * return void.
 */
static void find_type(char *buff, int type)
{
	log_debug("Entry:%s", __func__);
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

	log_debug("EXIT:%s", __func__);
}

/* process the list command
 *
 * @param[out]: buff, processed output.
 * return void.
 */
static void process_ls(char *buff)
{
	int offset = 0;
	DIR *d;
	struct dirent de, *dir;
	char type[10];
	struct stat st;
	
	log_debug("Entry:%s", __func__);

	memset(type, '\0', 10);
	d = opendir(".");
	if (d == NULL) {
		log_error("cannot open current directory \".\" ");
		return;
	}

	offset = sprintf(buff, "file:\t type:\t data:\n");
	if(d)
	{
		while (readdir_r(d, &de, &dir) == 0 && dir != NULL)
		{
			if(!strncmp(de.d_name, ".", 2) || !strncmp(de.d_name, "..", 3)) {
				continue; /* do want to expose whole file system to cleint */
			}

			find_type(type, de.d_type);
			stat(de.d_name, &st);
			offset += sprintf(buff + offset,"%s\t %s\t %s\n",
					 de.d_name, type, ctime(&st.st_ctime));
			log_debug("<%s>:type<%s>:data<%s>", de.d_name, type, ctime(&st.st_ctime));
			memset(type, '\0', 10);

		}
		closedir(d);
	}

	log_debug("EXIT:%s", __func__);
}

int create_socket(unsigned short port)
{
	int ret = 0;
	int opt = 1;
	int sock_fd; /* Socket descriptor for server */
	struct sockaddr_in server_addr; /* Local address */

	log_debug("Entry:%s", __func__);
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
		log_fatal("setsockopt failed");
		exit(1); 
	} 

	/* Bind to the local address */
	ret = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	assert(ret == 0);

	/* Mark the socket so it will listen for incoming connections */
	ret = listen(sock_fd, MAXPENDING);
	assert(ret == 0);

	log_debug("EXIT:%s", __func__);
	return sock_fd;
}

int accept_connection(int sock_fd)
{
	log_debug("Entry:%s", __func__);
	int comm_fd = 0;
	struct sockaddr_in client_addr; /* Client address */
	unsigned int client_len; /* Length of client address data structure */

	comm_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);	
	assert(comm_fd > 0);

	log_debug("EXIT:%s", __func__);
	return comm_fd;
}

void handle_client(int comm_fd)
{
	int ret = 0;
	char cmd_buf[BUFFER]; /* Buffer for echo string */
	int len = 0;
	char tmp[1024]; /* TODO use protobuf of serailizatio, or other library*/

	log_debug("Entry:%s", __func__);

	/* Receive cmd from client */
	while (1) {
		memset(cmd_buf, '\0', BUFFER);
		(void)recv(comm_fd, cmd_buf, BUFFER, MSG_NOSIGNAL);
		ret = cmd_parse(cmd_buf);
		memset(tmp, '\0', 1024);
		switch (ret){
			case CMD_LS:
				process_ls(tmp);	
				if (tmp == NULL) {
					strcpy(tmp, "server can't open curdir");
				}
				break;
			case CMD_CHDDIR:
				strcpy(tmp, "command not supported");
			/*	process_cd(tmp); */
				break;
			case CMD_BYE:
				strncpy(tmp, "bye", 3);
				send(comm_fd, tmp, sizeof(tmp), 0);
				sleep(1);
				goto out;
			default:
				log_info("command not supported");
				strcpy(tmp, "command not supported");
		}
		(void)send(comm_fd, tmp, strlen(tmp), 0);
		sleep(1);
	}
out:
	close(comm_fd); /* Close client socket */
	log_debug("EXIT:%s", __func__);
}
