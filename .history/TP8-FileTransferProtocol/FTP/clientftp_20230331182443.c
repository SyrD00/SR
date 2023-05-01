#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, filename[MAXLINE];
    rio_t rio;
     char buf[MAXLINE];
    ssize_t bytes_read;
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);

    clientfd = Open_clientfd(host, port);

    Rio_readinitb(&rio, clientfd);

    /* read filename from user */
    printf("Enter filename: ");
    if (Fgets(filename, MAXLINE, stdin) == NULL) {
        printf("no filename entered\n");
        Close(clientfd);
        exit(0);
    }

    /* send filename to server */
    //On envoie au serveur le nom du fichier de taille strlen(filename) tapé sur l'entrée standart par l'utilisateur.
    Rio_writen(clientfd, filename, strlen(filename));

    /* receive contents of file from server and print to stdout */
   
    while ((bytes_read = Rio_readlineb(&rio, buf, MAXLINE)) > 0) {
        Fputs(buf, stdout);
    }

    Close(clientfd);

    return 0;
}
