


#include "csapp.h"

#define MAX_NAME_LEN 256
#define NPROC 2 // nombre de processus exécutants
#define MAX_FILE_PATH_LEN 10000

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


    char buf[MAXLINE];
    ssize_t bytes_read;
    ssize_t bytes_written;

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
                

                //Initialisation de la structure “rio” pour la lecture du fichier
                Rio_readinitb(&rio, connfd);
                /*On lit  le contenu du fichier filename  par paquets de
                 taille MAXLINE et les envoie au client via la onnexion connfd
                */
                if ((n = Rio_readlineb(&rio, filename, MAXLINE)) == 0) {
                    printf("no filename received\n");
                    Close(connfd);//fermer le socket de connexion
                    continue;
                }
                
                if (filename[n-1] == '\n') {
                    filename[n-1] = '\0';
                }

                
                // Vérifier si le fichier demandé existe dans le répertoire du serveur
                if (access(filename, F_OK) == -1) {
                    printf("File %s does not exist in the server directory\n", filename);
                    Close(connfd);
                    continue;
                }

                 //Créer un répertoire nommé "FichierClient" si celui-ci n'existe pas encore
                 //777: tous les utilisateurs ont les droits de lecture, écriture et exécution idem pour les groupes et les autres
                //if (mkdir("FichierClient", 0777) == -1 && errno != EEXIST) {
                  //  perror("Error creating directory");
                    //exit(1);
                //}
                

                //char client_file_path[MAX_FILE_PATH_LEN];
                //Créer le chemin du fichier dans le dossier "FichierClient"
                //snprintf(client_file_path, sizeof(client_file_path), "FichierClient/%s", filename);
               






               /* open file in mode write */
                int filefd = Open(client_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                  // Ouvrir le fichier demandé en lecture binaire
                int filefd = Open(filename, );
                 if (filefd == -1) {
                    printf("File %s does not exist in server directory\n", filename);
                    Close(connfd);
                    continue;
                }

                // Créer un répertoire nommé "FichierClient" s'il n'existe pas encore
                if (mkdir("FichierClient", 0777) == -1 && errno != EEXIST) {
                    perror("Error creating directory");
                    exit(1);
                }

                // Créer le chemin du fichier dans le dossier "FichierClient"
                char client_file_path[MAX_FILE_PATH_LEN];
                snprintf(client_file_path, sizeof(client_file_path), "FichierClient/%s", filename);

                // Ouvrir le fichier dans lequel écrire le contenu du fichier demandé
                FILE *fileptr = fopen(client_file_path, "wb");
                if (fileptr == NULL) {
                    printf("Error creating file in client directory\n");
                    Close(connfd);
                    Close(filefd);
                    continue;
                }

                //Tout ce qui est lu dans le fichier est stocké dans la variable “buf” et envoyé au client.
               
               while ((bytes_read = Rio_readnb(&rio, buf, MAXLINE)) > 0) {
                    
                    bytes_written=write(filefd, buf, bytes_read);//écrire les données reçues dans le fichier
                    if (bytes_written != bytes_read) {
                         printf("error writing to client\n");
                        Close(connfd);//fermer le socket de connexion
                        Close(filefd);//fermer le fichier
                        continue;
                    }
                }
                //fermer le fichier
                Close(filefd);
                
                /* print message indicating file has been received */
                printf("File %s has been received.\n", filename);

                /* open the file again, this time in read binary mode */
                filefd = Open(filename, O_RDONLY);
                while ((bytes_read = read(filefd, buf, MAXLINE)) > 0) {
                    /* send the contents of the file to the client */
                    Rio_writen(connfd, buf, bytes_read);
                }

                //fermer le fichier
                Close(filefd);

                /* print message indicating file has been sent to client */
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
