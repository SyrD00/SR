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

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];

    char filename[MAXLINE];
    size_t n;
    rio_t rio;

    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        exit(0);
    }

    clientlen = (socklen_t)sizeof(clientaddr);
    listenfd = Open_listenfd(2121);

    // Créer le pool de processus exécutants
    for (int i = 0; i < NPROC; i++) {
        if (Fork() == 0) {
            while (1) {
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

                // Déterminer le nom du client
                Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAX_NAME_LEN, 0, 0, 0);

                // Déterminer la représentation textuelle de l'adresse IP du client
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string, INET_ADDRSTRLEN);

                printf("Server connected to %s (%s)\n", client_hostname, client_ip_string);

                // Initialisation de la structure "rio" pour la lecture du fichier
                Rio_readinitb(&rio, connfd);

                // Lire le nom du fichier envoyé par le client
                if ((n = Rio_readlineb(&rio, filename, MAXLINE)) == 0) {
                    printf("No filename received\n");
                    Close(connfd);
                    continue;
                }
                
                // Enlever le caractère '\n' à la fin du nom de fichier, s'il est présent
                if (filename[n-1] == '\n') {
                    filename[n-1] = '\0';
                }

                // Ouvrir le fichier en mode lecture
                int filefd = Open(filename, O_RDONLY);
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

                // Ouvrir le fichier dans le répertoire "FichierClient" en mode écriture binaire
                FILE *fileptr = fopen(client_file_path, "wb");
                if (fileptr == NULL) {
                    printf("Error creating file %s in FichierClient directory\n", filename);
                    Close(connfd);
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
