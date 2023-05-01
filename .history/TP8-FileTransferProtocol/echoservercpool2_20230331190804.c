/*
 * echoserver.c - A sequential echo server
 */
#include "csapp.h"

#define MAX_NAME_LEN 256
#define NCONN 4 // nombre maximal de connexions simultanées

void echo(int connfd);

int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    fd_set read_set, ready_set;
    int maxfd, nconn = 0;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);
    FD_ZERO(&read_set);
    FD_SET(listenfd, &read_set);
    maxfd = listenfd;

    while (1) {
        ready_set = read_set;
        Select(maxfd+1, &ready_set, NULL, NULL, NULL);
        if (FD_ISSET(listenfd, &ready_set)) {
            if (nconn < NCONN) {
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                          INET_ADDRSTRLEN);
                printf("server connected to %s (%s)\n", client_hostname,
                       client_ip_string);
                echo(connfd);
                Close(connfd);
                nconn++;
            }
        }
        if (nconn == NCONN) {
            for (int i = 0; i <= maxfd; i++) {
                if (i == listenfd) continue;
                if (FD_ISSET(i, &ready_set)) {
                    nconn--;
                }
            }
        }
    }

    return 0;
}
