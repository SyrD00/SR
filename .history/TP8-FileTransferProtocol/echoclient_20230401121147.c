#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd;
    char *host, filename[MAXLINE];
    rio_t rio;

     char buf[MAXLINE];
    ssize_t bytes_read;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host> \n", argv[0]);
        exit(0);
    }
    host = argv[1];

    clientfd = Open_clientfd(host,2121);

    Rio_readinitb(&rio, clientfd);

    /* read filename from user */
    printf("Enter filename: ");
    if (Fgets(filename, MAXLINE, stdin) == NULL) {
        printf("no filename entered\n");
        Close(clientfd);
        exit(0);
    }

    /* send filename to server */
    Rio_writen(clientfd, filename, strlen(filename));

    /* receive contents of file from server and write to file */
    FILE *fileptr;
    fileptr = fopen(filename, "wb"); //ouvrir le fichier en mode écriture binaire
    while ((bytes_read = Rio_readlineb(&rio, buf, MAXLINE)) > 0) {
        fwrite(buf, sizeof(char), bytes_read, fileptr); //écrire les données reçues dans le fichier
    }
    fclose(fileptr); //fermer le fichier

    printf("File %s downloaded successfully.\n", filename);

    Close(clientfd);

    return 0;
}
