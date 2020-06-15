/* File Name: server.c
 * Description: server implemention.
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"
#include "server_impl.h"
#include "log.h"

#define LOG_PATH		"/var/log/server.log"

struct thread_args { 		/* Structure of arguments to pass to client thread */
	int client_sock; 	/* socket descriptor for client */
};

void *worker(void *thread_args);

int log_init(void)
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
	int client_sock = 0;
	struct thread_args *thread_args; 	
	unsigned short port;
	pthread_t thread_id;

	/* log initialization */
	ret = log_init();

	log_info("Server started\n");

	port = 5432;

	/* create socket */
	server_sock = create_socket(port);

	for(;;) { 	/* Run forever */
		client_sock = accept_connection(server_sock); 
		assert(client_sock > 0);

		/* Create separate memory for client argument */
		thread_args = malloc(sizeof(struct thread_args));
		assert(thread_args != NULL);	
		thread_args->client_sock = client_sock;

		/* Create client thread */
		ret = pthread_create(&thread_id, NULL, worker, (void *)thread_args);
		assert(ret == 0);
	}
	return 0;
}

void *worker(void *thread_args)
{
	log_debug("Entry: %s\n", __func__);
	int client_sock; /* Socket descriptor for client connection */

	pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */

	/* Extract socket file descriptor from argument */
	client_sock = ((struct thread_args *)thread_args)->client_sock;

	free(thread_args); /* Deallocate memory for argument */

	handle_client(client_sock);
	log_debug("Exit: %s\n", __func__);
	return (NULL);
}
