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

    ssize_t n;
    char buf[MAXLINE];
    n = Rio_readnb(&rio, buf, MAXLINE); // Read the first chunk of data from the server

    
    /* Check if the server sent an error message */
    char error_msg[] = "FILE_NOT_FOUND";
    if (strncmp(buf, error_msg, strlen(error_msg)) == 0) {
        printf("File %s not found on the server.\n", filename);
        Close(clientfd);
        exit(0);
    }

    // Open the file only if the server didn't send an error message
    FILE *fileptr;
    fileptr = fopen(client_file_path, "ab"); // open file in write binary mode


        // Decodage du contenu en base64
    char decoded_buf[MAXLINE];
    int decoded_len = b64_pton(buf, (unsigned char*)decoded_buf, sizeof(decoded_buf));

    // Écrire le contenu décodé dans le fichier
    fwrite(decoded_buf, sizeof(char), decoded_len, fileptr);



    // Write the first chunk of data to the file
    fwrite(buf, sizeof(char), n, fileptr);

    // Continue reading and writing the rest of the file
    while ((n = Rio_readnb(&rio, buf, MAXLINE)) > 0) {
        fwrite(buf, sizeof(char), n, fileptr); // write received data to the file
    }

    fclose(fileptr); // close the file

    printf("File %s downloaded successfully.\n", filename);

    Close(clientfd);

    return 0;
}
