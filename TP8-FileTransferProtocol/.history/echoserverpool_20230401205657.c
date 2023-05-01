#include "csapp.h"

#define MAX_NAME_LEN 256
#define NPROC 2 // nombre de processus exécutants
#define MAX_FILE_PATH_LEN 10000

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

                printf("server connected to %s (%s)\n", client_hostname, client_ip_string);

                //Initialisation de la structure “rio” pour la lecture du fichier
                Rio_readinitb(&rio, connfd);

                /* On lit le nom du fichier envoyé par le client */
                if ((n = Rio_readlineb(&rio, filename, MAXLINE)) == 0) {
                    printf("no filename received\n");
                    Close(connfd); //fermer le socket de connexion
                    continue;
                }

                /* On retire le caractère de saut de ligne à la fin du nom de fichier */
                if (filename[n-1] == '\n') {
                    filename[n-1] = '\0';
                }

                /* Vérifier si le fichier demandé existe dans le répertoire du serveur */
                    /* Vérifier si le fichier demandé existe dans le répertoire "FileServeur" */
                char server_file_path[MAX_FILE_PATH_LEN];
                //On construit le chemin du fichier à partir du nom du fichier do
                snprintf(server_file_path, sizeof(server_file_path), "FichiersServeur/%s", filename);
               if (access(server_file_path, F_OK) == -1) {
                  printf("File %s does not exist in the server directory\n", server_file_path);
                    // Send an error message to the client
                    char error_msg[] = "FILE_NOT_FOUND";
                    Rio_writen(connfd, error_msg, strlen(error_msg));
                    Close(connfd);
                    break;
                }


                /* Ouvrir le fichier en mode lecture */
                  /* open the file in read mode */
                int filefd = Open(server_file_path, O_RDONLY, 0);
                if (filefd == -1) {
                    perror("Error opening file");
                    Close(connfd);
                    continue;
                }

                /* send the contents of the file to the client */
                ssize_t bytes_read;
                char buf[MAXLINE];
               
                while ((bytes_read = read(filefd, buf, MAXLINE)) > 0) {
                    Rio_writen(connfd, buf, bytes_read);
                }

                /* close the file */
                Close(filefd);

                printf("File %s has been sent to the client.\n", filename);

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