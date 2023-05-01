/*Serveur concurrent ECHO: la estion des processus dans une application client-serveur


iprogrammé). Un processus veilleur attend les requêtes des clients. Lorsqu’une requête
arrive, le veilleur la fait traiter par un exécutant, puis se remet en attente. Il y a deux
méthodes pour gérer les exécutants qui sont considérées dans les deux sections suivantes.
2.1 Création dynamique de processus
Une première méthode consiste, pour le veilleur, à créer un nouveau processus exécutant (par Fork()) pour servir chaque nouvelle demande de connexion.
1
Noter que les exécutants sont lancés en travail de fond, et passent à l’état zombi
quand ils se terminent. Prévoir leur élimination par le veilleur (utiliser le traitant du signal
SIGCHLD, puisque le veilleur ne peut pas se bloquer en attendant la fin des exécutants).



*/