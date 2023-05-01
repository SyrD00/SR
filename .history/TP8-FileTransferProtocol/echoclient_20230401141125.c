#include "csapp.h"
#define MAX_FILE_PATH_LEN 10000
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

    clientfd = Open_clientfd(host, 2121);

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
    char client_file_path[MAX_FILE_PATH_LEN];
    snprintf(client_file_path, sizeof(client_file_path), "FichierClient/%s", filename);

    


    if (access(filename, F_OK) == -1) {
    printf("File %s does not exist on server.\n", filename);
    Close(clientfd);
    exit(0);
}

/* open f
    FILE *fileptr;
    fileptr = fopen(client_file_path, "wb"); // open file in write binary mode
    while ((bytes_read = Rio_readlineb(&rio, buf, MAXLINE)) > 0) {
        fwrite(buf, sizeof(char), bytes_read, fileptr); // write received data to the file
    }
    fclose(fileptr); // close the file

    printf("File %s downloaded successfully.\n", filename);

    Close(clientfd);

    return 0;
}
