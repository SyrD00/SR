/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"


int main()
{
	Signal(SIGCHLD, sigchld_handler); // installation du traitant SIGCHLD
	while (1) {
		struct cmdline *l;
		int i, j;

		//int longueur_commande;
		//int dernier_caractere_commande;
		//int nb=nombre_commande(l->seq);

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
			
		}
		if(l->out!=NULL){
			redirection_sortie(l->out);
			
		}
		   
      	//interprete(l->seq[0]);
			
		/*
		if(nombre_commande(l->seq)==2)
			execute_2command(l->seq[0], l->seq[1]);
		if(nombre_commande(l->seq)==3)
			execute_3command(l->seq[0], l->seq[1], l->seq[2]);
		*/
	
		
		
		   

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			
			if(strcmp(cmd[0], "quit") == 0) 
				quitter();
			
			if(l->bg) 
        		execute_background(cmd);
			else{
				execute_Ncommand(l->seq);
				printf("\n");
			}
			printf("seq[%d]: ", i);
			
			for (j=0; cmd[j]!=0; j++) {
				printf("%s ", cmd[j]);
			}
			printf("\n");
			
		}
		
    	
       
		
		
							
		
	}
}
