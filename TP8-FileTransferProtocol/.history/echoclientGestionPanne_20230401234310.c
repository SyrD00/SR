#include "csapp.h"
#include <string.h>

#define MAX_FILE_PATH_LEN 10000

typedef struct {
    char filename[32];
    off_t offset;
} file_request;

int main(int argc, char **argv) {
    int clientfd;
    char *host;
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];

    clientfd = Open_clientfd(host, 2121);
    Rio_readinitb(&rio, clientfd);

    while (1) {
        printf("Enter filename (or type 'bye' to quit): ");
        char filename[32];
        if (Fgets(filename, 1000, stdin) == NULL) {
            printf("no filename entered\n");
            Close(clientfd);
            exit(0);
        }
        size_t len = strlen(filename);
            if (filename[len - 1] == '\n') {
                filename[len - 1] = '\0';
            }


        if (strncmp(filename, "bye", 3) == 0) {
            Rio_writen(clientfd, filename, strlen(filename));
            break;
        }

        char client_file_path[MAX_FILE_PATH_LEN];
        snprintf(client_file_path, sizeof(client_file_path), "FichierClient/%s", filename);

        off_t file_offset = 0;
        FILE *fileptr = fopen(client_file_path, "ab");
        if (fileptr != NULL) {
            fseek(fileptr, 0, SEEK_END);
            file_offset = ftell(fileptr);
        }

        file_request req;
        strncpy(req.filename, filename, 1000);
        req.offset = file_offset;
        Rio_writen(clientfd, (char *)&req, sizeof(file_request));

        ssize_t n;
        char buf[1000];
        n = Rio_readnb(&rio, buf, 1000);

        char error_msg[] = "FILE_NOT_FOUND";
        if (strncmp(buf, error_msg, strlen(error_msg)) == 0) {
            printf("File %s not found on the server.\n", filename);
            fclose(fileptr);
            continue;
        }

        if (fileptr == NULL) {
            fileptr = fopen(client_file_path, "wb");
        }

        fwrite(buf, sizeof(char), n, fileptr);

        while ((n = Rio_readnb(&rio, buf, 1000)) > 0) {
            fwrite(buf, sizeof(char), n, fileptr);
        }

        fclose(fileptr);
        printf("File %s downloaded successfully.\n", filename);
    }

    Close(clientfd);
    return 0;
}
/*
Ce code client modifié enverra la demande de fichier avec l’offset pour permettre au serveur de reprendre le 
transfert là où il s’est arrêté en cas de panne du client. Le serveur enverra le fichier à partir de cet offset,
 et le client l’écrira dans le fichier local à partir de cet offset également.*/