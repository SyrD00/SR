
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

    char filename[MAXLINE];
    size_t n;
    rio_t rio;

   if(argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
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
                

                Rio_readinitb(&rio, connfd);
                if ((n = Rio_readlineb(&rio, filename, MAXLINE)) == 0) {
                    printf("no filename received\n");
                    Close(connfd);//fermer le socket de connexion
                    continue;
                }
                
                if (filename[n-1] == '\n') {
                    filename[n-1] = '\0';
                }

               /* open file in mode write */
                int filefd = Open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                char buf[MAXLINE];
                ssize_t bytes_read;
                 ssize_t bytes_written;

                //Tout ce qui est lu dans le fichier est stocké dans la variable “buf” et envoyé au client
                while ((bytes_read = read(filefd, buf, MAXLINE)) > 0) {
                    
                    bytes_written=write(filefd, buf, bytes_read);//écrire les données reçues dans le fichier
                    if (bytes_written != bytes_read) {
                        printf("error writing to file\n");
                        Close(connfd);//fermer le socket de connexion
                        Close(filefd);//fermer le fichier
                        continue;
                    }
                }
                //fermer le fichier
                Close(filefd);
                
                /* print message indicating file has been received */
                printf("File %s has been received.\n", filename);
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

