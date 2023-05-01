/*
 * echoclient.c - An echo client
 */

#include "csapp.h"



#define MAX_NAME_LEN 256
int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    while (1) {
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);

        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                  INET_ADDRSTRLEN);

        printf("server connected to %s (%s)\n", client_hostname,
               client_ip_string);

        char filename[MAXLINE];
        size_t n;

        rio_t rio;
        /*Initia
        */
        Rio_readinitb(&rio, connfd);

        /* read filename from client */
        if ((n = Rio_readlineb(&rio, filename, MAXLINE)) == 0) {
            printf("no filename received\n");
            Close(connfd);
            continue;
        }

        /* remove newline character */
        if (filename[n-1] == '\n') {
            filename[n-1] = '\0';
        }

        /* open file and send contents to client */
        int filefd = Open(filename, O_RDONLY, 0);
        char buf[MAXLINE];
        ssize_t bytes_read;
        while ((bytes_read = Read(filefd, buf, MAXLINE)) > 0) {
            Rio_writen(connfd, buf, bytes_read);
        }

        Close(filefd);
        Close(connfd);
    }

    return 0;
}
