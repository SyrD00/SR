#include "csapp.h"
#include <sys/socket.h>
#include <netinet/in.h>

void echo(int connfd);

void *echo_thread(void *arg) {
    int connfd = *((int *)arg);
    free(arg); // libérer l'espace alloué pour le pointeur

    /* Gérer la connexion entrante */
    echo(connfd);

    /* Fermer la connexion et terminer le thread */
    Close(connfd);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int listenfd, *connfdp, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    pthread_t tid;
    
    /* Initialisation de listenfd et port */
    listenfd = Open_listenfd(port);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfdp = malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* Créer un nouveau thread pour gérer la connexion entrante */
        pthread_create(&tid, NULL, echo_thread, connfdp);
    }
    exit(0);
}

void echo(int connfd) {
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %u bytes\n", (unsigned int)n);
        Rio_writen(connfd, buf, n);
    }
}
