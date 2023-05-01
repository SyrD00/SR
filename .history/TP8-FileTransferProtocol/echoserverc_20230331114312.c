/*Serveur concurrent ECHO: Gestion des processus dans une application client-serveur

Dans cette architecture, le serveur est un processus “veilleur” qui attend les demandes de connexion
des clients. Lorsqu’une demande de connexion arrive, le serveur crée un nouveau processus “exécutant”
pour gérer la demande.

2.1 Création dynamique de processus
ans cette méthode, les processus exécutants sont créés dynamiquement en utilisant la fonction fork()
. Chaque nouveau processus exécutant est responsable de gérer une demande de connexion spécifique, en lisant les données envoyées par le client et en y répondant.


*/