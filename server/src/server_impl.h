#ifndef __SERVER_IMPL_H
#define __SERVER_IMPL_H

int create_socket(unsigned short port);

int accept_connection(int sockfd);

void handle_client(int clntSocket);

#endif /* __SERVER_IMPL_H */
