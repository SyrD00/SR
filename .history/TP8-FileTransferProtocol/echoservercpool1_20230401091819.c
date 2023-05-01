
#include "csapp.h"

#define MAX_NAME_LEN 256
#define NPROC 2 // nombre de processus exécutants

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
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];

   if(argc != 1) {
        fprintf(stderr, 
        exit(0);
    }


    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(2121);

    /* Installation du signal handler pour SIGCHILD */
    Signal(SIGCHLD, sigchld_handler);
    
    // créer le pool de processus exécutants
    for (int i = 0; i < NPROC; i++) {
        if (Fork() == 0) {
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
                  
                echo(connfd);

                Close(connfd);
            }
            exit(0);
        }
    }

    while (1) {
        pause(); // le processus veilleur ne fait rien, il se contente d'attendre des signaux
    }

    return 0;
}

