/* File Name: server.c
 * Description: server implemention.
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <signal.h> /* signal */
#include "server_impl.h"
#include "log.h"

#define LOG_PATH		"/var/log/server.log"

struct thread_args { 		/* Structure of arguments to pass to client thread */
	int client_sock; 	/* socket descriptor for client */
};

void *worker(void *thread_args);

/* initialzie log
 * 
 * parma: void.
 *
 * return 0 for success else errorno
 */
static int log_init(void)
{
	int rc = 0;

	FILE *log_file = fopen(LOG_PATH, "w+");
	if (log_file == NULL) {
		fprintf(stderr, "Can't open %s fil\n", LOG_PATH);
		rc = errno;
	}  

	log_set_fp(log_file);
	log_set_quiet(1);

	return rc;
}

int main()
{
	int ret = 0;
	int server_sock = 0;
	struct thread_args *thread_args; 	
	unsigned short port;
	pthread_t thread_id;
	fd_set readfds; 

	signal(SIGPIPE, SIG_IGN); /* ignore sigpipe signal */

	/* log initialization */
	ret = log_init();
	if (ret != 0 ) {
		fprintf(stderr, "log init failed\n");
		goto out;
	}

	log_info("Server started\n");

	/* port number, can be read for conf file */
	port = 5432;

	/* create socket */
	server_sock = create_socket(port);

	for(;;) { 	/* Run forever */
		FD_ZERO(&readfds);
		FD_SET(server_sock, &readfds);
		log_info("blocked on select call");

		select(server_sock + 1, &readfds, NULL, NULL, NULL);

		if (FD_ISSET(server_sock, &readfds)) {
			log_debug("New connection recived");


			int client_sock = accept_connection(server_sock); 
			assert(client_sock > 0);

			/* Create separate memory for client argument */
			thread_args = malloc(sizeof(struct thread_args));
			assert(thread_args != NULL);	
			thread_args->client_sock = client_sock;

			/* Create client thread */
			ret = pthread_create(&thread_id, NULL, worker, (void *)thread_args);
			assert(ret == 0);
		}
	}
out:
	return 0;
}

/* Thread worker
 *
 * param[in]: args from main. 
 * return void.
 */
void *worker(void *thread_args)
{
	log_debug("Entry: %s\n", __func__);
	int client_sock; /* Socket descriptor for client connection */

	pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */

	/* Extract socket file descriptor from argument */
	client_sock = ((struct thread_args *)thread_args)->client_sock;

	free(thread_args); /* Deallocate memory for argument */
	/* check handle_cleint is reentrant function, if not have synchronization */
	handle_client(client_sock);
	log_debug("Exit: %s\n", __func__);
	return (NULL);
}
