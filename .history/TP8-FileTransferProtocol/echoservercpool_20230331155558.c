/**
**Dans la méthode de pool de processus, un certain nombre fixe de processus exécutants est créé à 
*l’avance. Lorsqu’une demande de connexion arrive, elle est traitée par l’un de ces processus, qui se
*remet ensuite en attente. Le veilleur, quant à lui, continue d’attendre de nouvelles demandes de
*connexion en utilisant la boucle while(1).

*TODO:Pour faire attendre et activer les exécutants, il y a deux solutions possibles :
*La primitive accept est exécutée par:
*? Le processus principal (le veilleur) qui attend de nouvelles demandes de connexion.
*?dans ce cas, le veilleur doit gérer une file de connexions en attente de traitement. 
*? Lorsqu’une connexion est acceptée, le veilleur doit la placer dans la file et réveiller un
*?processus exécutant qui sera chargé de la traiter.
*!les processus exécutants : dans ce cas, chaque processus exécutant doit attendre une nouvelle 
*!connexion en utilisant la primitive accept. Lorsqu’une nouvelle connexion arrive, le processus la
*! traite et se remet en attente de la prochaine connexion.

**NB:La solution la plus simple à réaliser dépend du contexte d’utilisation. 
*?Si le nombre de connexions est très élevé, il peut être plus efficace d’utiliser la 1er solution 
*?pour éviter d’avoir trop de processus en attente. 
Si le nombre de connexions est plus faible, la seconde solution peut être plus simple à implémenter.



*/