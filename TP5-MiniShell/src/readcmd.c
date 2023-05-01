/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "readcmd.h"
#include "csapp.h"
#include <stdbool.h>

//Cette fonction memory_error permet de gérer les erreurs de malloc et realloc
static void memory_error(void)
{
	errno = ENOMEM;
	perror(0);
	exit(1);
}

//La fonction xmalloc sert à allouer de la mémoire et à gérer les erreurs
static void *xmalloc(size_t size)
{
	void *p = malloc(size);
	if (!p) memory_error();
	return p;
}


static void *xrealloc(void *ptr, size_t size)
{
	void *p = realloc(ptr, size);
	if (!p) memory_error();
	return p;
}


/* Read a line from standard input and put it in a char[] */
static char *readline(void)
{
	size_t buf_len = 16;
	char *buf = xmalloc(buf_len * sizeof(char));

	if (fgets(buf, buf_len, stdin) == NULL) {
		free(buf);
		return NULL;
	}
	
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout); //permet de vider le buffer de sortie car on quitte le programme
	    exit(0);
	}

	do {
		size_t l = strlen(buf);
		if ((l > 0) && (buf[l-1] == '\n')) {//si on a lu une ligne
			l--;
			buf[l] = 0;
			return buf;
		}
		if (buf_len >= (INT_MAX / 2)) memory_error();
		buf_len *= 2;
		buf = xrealloc(buf, buf_len * sizeof(char));
		if (fgets(buf + l, buf_len - l, stdin) == NULL) return buf;
	} while (1);
}


/* Split the string in words, according to the simple shell grammar. */
static char **split_in_words(char *line)
{
	char *cur = line;
	char **tab = 0;
	size_t l = 0;
	char c;

	while ((c = *cur) != 0) {
		char *w = 0;
		char *start;
		switch (c) {
		case ' ':
		case '\t':
			/* Ignore any whitespace */
			cur++;
			break;
		case '<':
			w = "<";
			cur++;
			break;
		case '>':
			w = ">";
			cur++;
			break;
		case '|':
			w = "|";
			cur++;
			break;
		case '&':
			w = "&";
			cur++;
			break;
		default:
			/* Another word */
			start = cur;
			while (c) {
				c = *++cur;
				switch (c) {
				case 0:
				case ' ':
				case '\t':
				case '<':
				case '>':
				case '&':
				case '|':
					c = 0;
					break;
				default: ;
				}
			}
			w = xmalloc((cur - start + 1) * sizeof(char));
			strncpy(w, start, cur - start);
			w[cur - start] = 0;
		}
		if (w) {
			tab = xrealloc(tab, (l + 1) * sizeof(char *));
			tab[l++] = w;
		}
	}
	tab = xrealloc(tab, (l + 1) * sizeof(char *));
	tab[l++] = 0;
	return tab;
}


static void freeseq(char ***seq)
{
	int i, j;

	for (i=0; seq[i]!=0; i++) {
		char **cmd = seq[i];

		for (j=0; cmd[j]!=0; j++) free(cmd[j]);
		free(cmd);
	}
	free(seq);
}


/* Free the fields of the structure but not the structure itself */
static void freecmd(struct cmdline *s)
{
	if (s->in) free(s->in);
	if (s->out) free(s->out);
	if (s->seq) freeseq(s->seq);

}


struct cmdline *readcmd(void)
{
	static struct cmdline *static_cmdline = 0;
	struct cmdline *s = static_cmdline;
	char *line;
	char **words;
	int i;
	char *w;
	char **cmd;
	char ***seq;
	size_t cmd_len, seq_len;
	

	line = readline();
	if (line == NULL) {
		if (s) {
			freecmd(s);
			free(s);
		}
		return static_cmdline = 0;
	}

	cmd = xmalloc(sizeof(char *));
	cmd[0] = 0;
	cmd_len = 0;
	seq = xmalloc(sizeof(char **));
	seq[0] = 0;
	seq_len = 0;

	words = split_in_words(line);
	free(line);

	if (!s)
		static_cmdline = s = xmalloc(sizeof(struct cmdline));
	else
		freecmd(s);
	s->err = 0;
	s->in = 0;
	s->out = 0;
	s->seq = 0;
	s->bg=false;

	i = 0;
	while ((w = words[i++]) != 0) {
		switch (w[0]) {
		case '<':
			/* Tricky : the word can only be "<" */
			if (s->in) {
				s->err = "only one input file supported";
				goto error;
			}
			if (words[i] == 0) {
				s->err = "filename missing for input redirection";
				goto error;
			}
			s->in = words[i++];
			break;
		case '>':
			/* Tricky : the word can only be ">" */
			if (s->out) {
				s->err = "only one output file supported";
				goto error;
			}
			if (words[i] == 0) {
				s->err = "filename missing for output redirection";
				goto error;
			}
			s->out = words[i++];
			break;
		case '|':
			/* Tricky : the word can only be "|" */
			if (cmd_len == 0) {
				s->err = "misplaced pipe";
				goto error;
			}

			seq = xrealloc(seq, (seq_len + 2) * sizeof(char **));
			seq[seq_len++] = cmd;
			seq[seq_len] = 0;

			cmd = xmalloc(sizeof(char *));
			cmd[0] = 0;
			cmd_len = 0;
			break;
		 case '&':
		 	//Si aprés avoir rencontré un & on rencontre un autre mot c'est une erreur d'emplacement du &.
             if (words[i] != 0){
                s->err = "misplaced background symbol";;
                goto error;
            }
           
    		s->bg = 1;
    break;

            break;
		default:
			cmd = xrealloc(cmd, (cmd_len + 2) * sizeof(char *));
			cmd[cmd_len++] = w;
			cmd[cmd_len] = 0;
		}
	}

	if (cmd_len != 0) {
		seq = xrealloc(seq, (seq_len + 2) * sizeof(char **));
		seq[seq_len++] = cmd;
		seq[seq_len] = 0;
	} else if (seq_len != 0) {
		s->err = "misplaced pipe";
		i--;
		goto error;
	} else
		free(cmd);
	free(words);
	s->seq = seq;
	return s;
error:
	while ((w = words[i++]) != 0) {
		switch (w[0]) {
		case '<':
		case '>':
		case '|':
		case '&':
			break;
		default:
			free(w);
		}
	}
	free(words);
	freeseq(seq);
	for (i=0; cmd[i]!=0; i++) free(cmd[i]);
	free(cmd);
	if (s->in) {
		free(s->in);
		s->in = 0;
	}
	if (s->out) {
		free(s->out);
		s->out = 0;
	}
	return s;
}


void quitter()
{
	exit(0);//permet de quitter le shell car exit(0) signifie que le programme s'est bien executé
}


void interprete(char **commande){
	//On doit créer un processus fils pour executer la commande et ainsi ne pas bloquer le shell
	pid_t pid = fork();
	if(pid == -1){
		gestion_erreur();
	}
	if(pid == 0){
		//On est dans le processus fils
		char *cmd = commande[0];
		//Les arguments sont commande a partir de l'index 1 jusqu'a la fin
		char **args = commande;
		
		execvp(cmd, args);
		
	}
	
}

void redirection_entree(char *in){
	if(in != NULL){
		//On ouvre le fichier en lecture seule
		int fd = open(in, O_RDONLY);

		dup2(fd, 0);//On duplique le descripteur de fichier fd dans le descripteur de fichier 0 qui correspond a stdin
		close(fd);
		
	}
	
}
void redirection_sortie(char *out){
	if(out != NULL){
		int fd = open(out, O_WRONLY|O_CREAT);
		dup2(fd, 1);
		close(fd);
		
	}
	else{
		fprintf(stderr, "Aucun fichier n'a été spécifié");
	}
}

void gestion_erreur(){
    switch(errno){
        case ENOENT:
            fprintf(stderr, "Commande introuvable\n");
            break;
        case EACCES:
            fprintf(stderr, "Permission refusée\n");
            break;
        case ENOTDIR:
            fprintf(stderr, "Le chemin d'accès vers le fichier exécutable est incorrect:Aucun fichier ou dossier de ce type\n");
            break;
        case EINVAL:
            fprintf(stderr, "Argument invalide\n");
            break;
        case ECHILD:
            fprintf(stderr, "Processus enfant inexistant\n");
            break;
        case EEXIST:
            fprintf(stderr, "Le fichier existe déjà\n");
            break;
        case EISDIR:
            fprintf(stderr, "Le fichier est un répertoire\n");
            break;
        case EMFILE:
            fprintf(stderr, "Trop de fichiers ouverts\n");
            break;
        case EINTR:
            fprintf(stderr, "Appel systémé Interrompu\n");
            break;
        case ESTRPIPE:
            fprintf(stderr, "Pipe brisé\n");
            break;
        case E2BIG:
            fprintf(stderr, "Trop d'arguments\n");
            break;
        case EBADFD:
            fprintf(stderr,"Mauvaus descripteur de fichier\n");
            break;
        case EFAULT:
            fprintf(stderr,"Argument avec une adresse invalide\n");
            break;
        default:
            fprintf(stderr, "Erreur inattendue\n");
            break;
    }
}


void execute_2command(char **commande1, char **commande2){

	char *cmd1 = commande1[0];
	
	char **args1 = commande1 ;
	
	char *cmd2 = commande2[0];
	
	char **args2 = commande2 ;
	int fd[2];
	
	
	
	//On crée un processus fils pour executer la 1er commande
	pid_t pid1 = fork();

    if (pipe(fd) == -1) {
       
        gestion_erreur();//EBADF
		
    }

    pid1 = fork();
    if (pid1 < 0) {
        
        gestion_erreur();
        
    } 
	else if(pid1 == 0){
		 close(fd[0]);
		//On veut copier l'entrée du tube sur la sortie standard.Donc la sortie de la 1ere commande est redirigée vers l'entrée du tube
		dup2(fd[1], 1); 
		//Si cmd1=Chemin d'accés inavalide, arg1 si invalide
		 if (execvp(cmd1, args1) == -1) {
            gestion_erreur();//EACCES ,ENOENT,ENOMEM ,EINVAL,EFAULT      
            
        }
	}
	//On crée un processus fils pour executer la 2eme commande
	pid_t pid2 = fork();
	
    if (pid2 < 0) {
       
        gestion_erreur();
       
    } else if(pid2 == 0){
		close(fd[1]);
		//On veut copier la sortie du tube sur l'entrée' standard
		dup2(fd[0], 0);
		
		 if (execvp(cmd2, args2) == -1) {
            gestion_erreur();
          
        }
	}
	
	else{
		
		 //On ferme les deux extrémités du tube dans le processus parent
           close(fd[0]);
		   close(fd[1]);
            

           //le processus parent attend que les processus fils se terminent
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
	}
	
}
int nombre_commande(char ***sequence_commande){
	int i = 0;
	while(sequence_commande[i] != NULL){
		i++;
	}
	return i;
}
void execute_3command(char **command1, char **command2, char **command3) {
    int fd[2], fd2[2];
	

    int status1, status2, status3;
    pid_t pid1, pid2, pid3;

    /* créer la première pipe */
    if (pipe(fd) == -1) {
    
        gestion_erreur();
      
    }

    pid1 = fork();
    if (pid1 < 0) {
       
        gestion_erreur();
        
    } 
	else if (pid1 == 0) { /* premier fils */
       
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
       // close(fd[1]);
        /* exécuter la première commande */
        if (execvp(command1[0], command1) == -1) {
            gestion_erreur();
         
        }
    }

    /* créer la deuxième pipe */
    if (pipe(fd2) == -1) {
     
        gestion_erreur();
        
    }

    pid2 = fork();
    if (pid2 < 0) {
        
        gestion_erreur();
       
    } else if (pid2 == 0) { /* deuxième fils */
		 close(fd[1]);
        /* rediriger l'entrée vers la première pipe */
        dup2(fd[0], STDIN_FILENO);
        /* rediriger stdout vers l'entrée de la deuxième pipe */
        dup2(fd2[1], STDOUT_FILENO);
        /*fermer les extrémités de la pipe inutilisées  */
        
       
       
        close(fd2[1]);
		

        /* exécuter la deuxième commande */
        if (execvp(command2[0], command2) == -1) {
            gestion_erreur();
          
        }
    }

    pid3 = fork();
    if (pid3 < 0) {
       
        gestion_erreur();
       
    } 
	else if (pid3 == 0) { /* troisième fils */
        /* rediriger l'entrée vers la deuxième pipe */
        dup2(fd2[0], STDIN_FILENO);
        /* fermer les extrémités de la pipe inutilisées */
       
        close(fd[1]);
       
        close(fd2[1]);
		
        /* exécuter la troisième commande */
        if (execvp(command3[0], command3) == -1) {
            gestion_erreur();
          
        }
    }

    /* fermer les extrémités de la pipe inutilisées dans le processus parent */
    close(fd[0]);
    close(fd[1]);
    close(fd2[0]);
    close(fd2[1]);

    /* attendre la fin des fils */
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    waitpid(pid3, &status3, 0);
}




void execute_Ncommand(char ***seq) {
    int num_cmds = nombre_commande(seq);
	
    

    int **fds = (int **) malloc(sizeof(int *) * num_cmds);
    // On crée un tableau de pipes pour chaque commande sauf la dernière
    for (int i = 0; i < num_cmds - 1; i++) {
        fds[i] = (int *) malloc(sizeof(int) * 2);
        if (pipe(fds[i]) < 0) {
            gestion_erreur();
          
        }
    }

    //On créé pour chaque commande un processus fils.Chaque commande sera exécutée dans un processus fils
    for (int i = 0; i < num_cmds; i++) {
        int pid = fork();
        if (pid < 0) {
           
			gestion_erreur();
           
        }
        
        else if (pid == 0) {
            /*i=0(1er commande)/Donc i>0 on n'est pas à la première commande
			 fds[i-1][0] =la sortie de la commande précédente
			*/
            if (i > 0) {
				//On redirige la sortie de la commande précédente vers l'entrée de la commande courante
                if (dup2(fds[i - 1][0], STDIN_FILENO) < 0) {
					gestion_erreur();
                    
                }
            }
            //Si on n'est pas à la dernière commande
            if (i < num_cmds - 1) {
				//On redirige la sortie de la commande courante vers l'entrée de la commande suivante
                if (dup2(fds[i][1], STDOUT_FILENO) < 0) {
                   
                    gestion_erreur();
                }
            }
            // On ferme tous les pipes dans le processus enfant
            for (int j = 0; j < num_cmds - 1; j++) {
                close(fds[j][0]);
                close(fds[j][1]);
            }
            // On exécute la commande correspondante
            if (execvp(seq[i][0], seq[i]) < 0) {
                gestion_erreur();
               
            }
        }
    }

    // On ferme tous les pipes dans le processus parent
    for (int i = 0; i < num_cmds - 1; i++) {
        close(fds[i][0]);
        close(fds[i][1]);
    }

    // On attend la fin de tous les processus enfants
    for (int i = 0; i < num_cmds; i++) {
        int status;
        wait(&status);
        if (!WIFEXITED(status)) {
            printf("La commande %d ne s'est pas terminé correctement et a le statut de sortie %d\n", i, WEXITSTATUS(status));
        } 
		//Si le processus a été interrompu par un signal
		else if (WIFSIGNALED(status)) {
            printf("La commande %d s'est terminé a cause du signal %d\n", i, WTERMSIG(status));
        }
    }

    // On libère la mémoire allouée pour les pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        free(fds[i]);
    }
    free(fds);
}


void execute_background(char **cmd){
  pid_t pid;
  int status;

  /* Créer un nouveau processus fils */
  pid = Fork();

  if (pid == 0) {
    

    /*On utilise SIG_IN pour ignorer les signaux(ici SIGINT et SIGSTP) pour éviter qu'ils ne perturbent l'exécution de la commande */
    Signal(SIGINT, SIG_IGN);
    Signal(SIGTSTP, SIG_IGN);

    
    execvp(cmd[0], cmd);

    gestion_erreur();
  } else if (pid > 0) {

   /*Waitpid(pid, &status, WNOHANG) =0 si le processus fils n'a pas terminé son exécution*/
    if (Waitpid(pid, &status, WNOHANG) == 0) {
      /*La commande est en cours d'exécution en arrière-plan */
      printf("Commande en cours d'exécution en arrière-plan\n");
    }
  } else {
		//Impossible de créer un nouveau processus fils*
		gestion_erreur();//ECHILD
		
  }
}


void sigchild_handler(int sig){
    pid_t pid;
    int status;
	/* le processus père peut interroger le statut de sortie du fils à l'aide des 
	fonctions WIFEXITED() et WEXITSTATUS(). */
	/*Pour éviter les zombies,il faut que le pére prend connaissance de la terminaison de ses fils */

	//Tant que il y a des fils (si pid=0,donc le fils n'a pas terminé son exécution)
    while((pid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0){
        if(WIFEXITED(status)){
            printf("Processus %d terminé avec status %d\n", pid, WEXITSTATUS(status));
        }else if(WIFSIGNALED(status)){
            printf("Processus %d terminé suite à la réception du signal %d\n", pid, WTERMSIG(status));
        }else if(WIFSTOPPED(status)){
            printf("Processus %d stoppé suite à la réception du signal %d\n", pid, WSTOPSIG(status));
        }else if(WIFCONTINUED(status)){
            printf("Processus %d redémarré\n", pid);
        }
    }
}

