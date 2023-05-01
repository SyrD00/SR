#include "csapp.h"

#define MAX_FILE_PATH_LEN 10000

int main(int argc, char **argv)
{
    int clientfd;
    char *host, filename[MAXLINE];
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];

    clientfd = Open_clientfd(host, 2121);

    Rio_readinitb(&rio, clientfd);

    while (1) {
        /* read filename from user */
        printf("Enter filename (or type 'bye' to quit): ");
        if (Fgets(filename, MAXLINE, stdin) == NULL) {
            printf("no filename entered\n");
            Close(clientfd);
            exit(0);
        }

        /* si l'utilisateu tape bye on envoie cette commande au serveur et on ferme la connexion */

        //On utiliser
        if (strncmp(filename, "bye", 3) == 0) {
            //nous envoyons la commande “bye” au serveur en utilisant la fonction Rio_writen
            Rio_writen(clientfd, filename, strlen(filename));
            break;
        }

        /* send filename to server */
        Rio_writen(clientfd, filename, strlen(filename));

        /* on reçoit le contenu du fichier du serveur et on l'écrit dans un fichier */
        char client_file_path[MAX_FILE_PATH_LEN];
        snprintf(client_file_path, sizeof(client_file_path), "FichierClient/%s", filename);

        ssize_t n;
        char buf[MAXLINE];
        n = Rio_readnb(&rio, buf, MAXLINE); // on lit le premier morceau de données du serveur

        /* Check if the server sent an error message */
        char error_msg[] = "FILE_NOT_FOUND";
        if (strncmp(buf, error_msg, strlen(error_msg)) == 0) {
            printf("File %s not found on the server.\n", filename);
            continue;
        }

        // on arrive ici uniquement si le serveur n'a pas envoyé un message d'erreur
        FILE *fileptr;
        fileptr = fopen(client_file_path, "wb"); // on ouvre le fichier en mode écriture binaire

        // on écrit le premier morceau de données dans le fichier
        fwrite(buf, sizeof(char), n, fileptr);

        // on continue la lecture et l'écriture du reste du fichier
        while ((n = Rio_readnb(&rio, buf, MAXLINE)) > 0) {
            fwrite(buf, sizeof(char), n, fileptr); // on écrit les données reçues dans le fichier créé précédemment en utilisant la fonctio
        }

        fclose(fileptr); //on ferme le fichier

        printf("File %s downloaded successfully.\n", filename);
    }

    Close(clientfd);

    return 0;
}
