/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"


int main()
{
	while (1) {
		struct cmdline *l;
		int i, j;

		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);
		if(l->in!=NULL){
			redirection_entree(l->in);
			//quitter();
		}
		if(l->out!=NULL){
			redirection_sortie(l->out);
			//quitter();
		}
			
		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			
			if(strcmp(cmd[0], "quit") == 0) 
				quitter();
			
			if (l->seq[i+1] != 0) { // Il y a une commande suivante dans la séquence, on doit créer un tube
       			char **next_cmd = l->seq[i+1];
				char **next_next_cmd = l->seq[i+2];
        		execute_2command(cmd, next_cmd);
				execute_3command(cmd, next__cmd);
       			i++; // On saute la commande suivante dans la boucle
            } 
			else {
        		interprete(cmd);
    		}


			printf("seq[%d]: ", i);
			for (j=0; cmd[j]!=0; j++) {
				printf("%s ", cmd[j]);
			}
			printf("\n");
		}
		

		
	}
}
