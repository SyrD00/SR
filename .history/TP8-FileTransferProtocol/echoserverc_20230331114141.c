/*Serveur concurrent ECHO: Gestion des processus dans une application client-serveur



2.1 Création dynamique de processus
Une première méthode consiste, pour le veilleur, à créer un nouveau processus exécutant (par Fork()) pour servir chaque nouvelle demande de connexion.
1
Noter que les exécutants sont lancés en travail de fond, et passent à l’état zombi
quand ils se terminent. Prévoir leur élimination par le veilleur (utiliser le traitant du signal
SIGCHLD, puisque le veilleur ne peut pas se bloquer en attendant la fin des exécutants).



*/