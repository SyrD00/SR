#include "csapp.h"

void echo(int connfd);
void sigchld_handler(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child process %d terminated\n", pid);
    }
}
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

    /* Installation du signal handler pour SIGCHILD */
    Signal(SIGCHLD, sigchld_handler);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfdp = malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* Créer un nouveau thread pour gérer la connexion entrante */
        pthread_create(&tid, NULL, echo_thread, connfdp);
    }
    exit(0);
}
