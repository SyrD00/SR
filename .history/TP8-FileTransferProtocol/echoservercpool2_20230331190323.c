#include "csapp.h"
#include <pthread.h>

#define MAX_NAME_LEN 256
#define NPROC 4 // nombre de processus exécutants
#define LISTENQ 1024 // taille de la file d'attente de connexions

void echo(int connfd);


void sigchld_handler(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child process %d terminated\n", pid);
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    int clients[NPROC];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    Signal(SIGCHLD, sigchld_handler);

    // initialiser les descripteurs de fichiers des clients à -1
    for (int i = 0; i < NPROC; i++) {
        clients[i] = -1;
    }

    // créer le pool de processus exécutants
    for (int i = 0; i < NPROC; i++) {
        if (Fork() == 0) {
            while (1) {
                // attendre une connexion
                Pthread_mutex_lock(&mutex);
                connfd = clients[i];
                clients[i] = -1;
                Pthread_mutex_unlock(&mutex);

                // traiter la connexion
                if (connfd >= 0) {
                    Getnameinfo((SA *) &clientaddr, clientlen,
                                client_hostname, MAX_NAME_LEN, 0, 0, 0);

                    Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                              INET_ADDRSTRLEN);

                    printf("server connected to %s (%s)\n", client_hostname,
                           client_ip_string);

                    echo(connfd);

                    Close(connfd);
                }
            }
            exit(0);
        }
    }

    // processus veilleur
    while (1) {
        // attendre une connexion
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        // ajouter la connexion à la file d'attente
        int i;
        for (i = 0; i < NPROC; i++) {
            Pthread_mutex_lock(&mutex);
            if (clients[i] < 0) {
                clients[i] = connfd;
                Pthread_mutex_unlock(&mutex);
                break;
            }
            Pthread_mutex_unlock(&mutex);
        }

        // si la file d'attente est pleine, fermer la connexion
        if (i == NPROC) {
            Close(connfd);
        }
    }

    return 0;
}
