## La class main
a) Fonction `prompt`
* Description : Cette fonction génère et affiche un prompt personnalisé pour l'utilisateur. Elle prend en compte la longueur du chemin d'accès actuel et le nombre de jobs en cours pour formater le prompt.
* Implémentation : Utilise snprintf pour formater le prompt et fprintf pour l'afficher. Elle gère les cas où le chemin est trop long pour le prompt.
b) `Fonction main`
* Rôle Principal : Point d'entrée de l'application. Gère le flux principal de l'exécution du programme, notamment la saisie des commandes utilisateur et leur traitement.
* Gestion des commandes : Utilise readline pour lire les entrées de l'utilisateur .
* Boucle principale : Une boucle while infinie est utilisée pour lire continuellement les entrées de l'utilisateur jusqu'à ce que la commande exit soit entrée.
* Gestion des jobs : Gère les jobs (des processus ou des tâches en arrière-plan) et met à jour leur état.
3. Structures de Données et Algorithmes
* Algorithmes : Des algorithmes pour la gestion des jobs et la mise à jour des états de ces jobs sont impliqués.
4. Bibliothèques Utilisées
* Readline : Pour la saisie et la gestion de l'historique des commandes.
* Autres Bibliothèques Système : Pour la gestion des fichiers et répertoires (dirent.h, sys/stat.h, etc.), et la gestion des processus (unistd.h, sys/wait.h).

## Gestion des Redirections et Pipes
1. Description Générale
Ce module est dédié à la gestion des redirections et des pipes de jsh. Il implémente plusieurs fonctions pour rediriger les entrées et sorties standards ainsi que pour gérer les pipes entre les commandes.
2. Fonctions Principales
a) `containsExactSubstring`
* Rôle : Vérifie si une chaîne de caractères contient un mot spécifique.
* Implémentation : Utilise strtok pour diviser la chaîne en mots et strcmp pour comparer chaque mot.
b) `appelRedirection`
* Fonctionnalité : Analyse les arguments de la commande pour détecter et gérer les redirections et les pipes.
* Processus : Identifie les symboles de redirection (<, >, >>, etc.) et les pipes (|), puis redirige les flux en conséquence.
c) Fonctions de Redirection
* redirectInStandard, redirectOutStandard, redirectOutEcrase, redirectOutConcat : Gèrent les redirections des entrées et sorties standards, y compris l'ajout et l'écrasement de fichiers.
* redirectErrStandard, redirectErrEcrase, redirectErrConcat : Spécifiques pour la redirection des erreurs standards vers des fichiers.
d) `redirectPipe`
* Rôle : Gère le piping entre deux commandes.
* Mécanisme : Utilise fork et pipe pour créer des processus enfants et redirige les sorties et entrées des commandes.
e) `substitution`
* Objectif : Devrait permettre la substitution de commande en utilisant un fichier FIFO temporaire.
* Processus : Crée deux processus enfants pour exécuter les commandes et substituer la sortie de l'une comme entrée de l'autre.
3. Stratégies et Algorithmes
* Gestion des Erreurs : Toutes les fonctions implémentent une gestion d'erreur, signalant les problèmes lors de l'ouverture des fichiers, la création de pipes, etc.
4. Dépendances et Bibliothèques
* Bibliothèques Système : Utilisation de fcntl.h, unistd.h, stdlib.h, stdio.h, etc., pour les opérations de fichiers et de processus.

## Traitement des Commandes et Gestion des Jobs
1. Structure de Commande
* Tableau de Commandes : Un tableau commands contenant des commandes internes, telles que pwd, cd, jobs, et kill, avec des pointeurs vers leurs fonctions respectives.
2. Fonctions Clés
a) `splitString`
* Rôle : Divise une chaîne de caractères en mots.
* Utilisation : Utilisée pour analyser les instructions entrées par l'utilisateur.
b) `appel`
* Fonction : Point central pour appeler les commandes internes et externes.
* Processus : Analyse l'instruction, gère les redirections, et détermine si la commande est interne ou externe.
c) Commandes Internes (pwd, cd, interogation, etc.)
* Implémentation : Chaque commande a sa propre fonction, gérant des tâches spécifiques comme changer de répertoire, afficher le répertoire de travail actuel, etc.
d) `cmd_externe`
* Fonctionnalité : Exécute des commandes externes en utilisant fork et execvp.
* Gestion des Processus : Gère les processus en arrière-plan et en avant-plan.
e) `cmd_jobs`, `cmd_kill`, `cmd_fg`
* Rôle : Gestion des jobs, notamment leur affichage, leur terminaison et leur reprise.
3. Gestion des Jobs
* init_jobs et update : Initialisation et mise à jour de l'état des jobs.
4. Gestion de l'Environnement
* Variable d'Environnement : Utilisation de variables d'environnement pour stocker des informations telles que le dernier code de retour (?).



## Gestion des Jobs
1. Vue d'Ensemble
Ce module implémente des fonctions avancées pour la gestion des jobs dans jsh, telles que l'ajout, la mise à jour, l'affichage et la suppression des jobs, ainsi que la gestion des signaux.
2. Structures et Fonctions Principales
a) Structures job_list et job_node
* Rôle : Stockent les informations sur les jobs actifs et terminés.
* Description : job_list est une liste chaînée de job_node, qui contient les détails de chaque job.
b) Gestion des Jobs
* `init_job`, `add_job_to_jobs`, `job_get_size` : Initialisent les jobs, ajoutent des jobs à la liste et retournent le nombre de jobs actifs.
* `print_jobs`, `print_job_list`, `printJob` : Affichent les informations des jobs.
c) Mise à Jour et Nettoyage
* `job_update` : Met à jour l'état des jobs et imprime les jobs terminés.
* `remove_done_killed_jobs` : Nettoie les jobs terminés ou tués de la liste des jobs actifs.
d) Manipulation des Jobs
* `kill_pid`, `kill_job, `fg_job` : Fonctions pour envoyer des signaux aux jobs, les arrêter, les reprendre ou les mettre en avant-plan.
3. Gestion des Signaux et États des Jobs
* Gestion des États : Les jobs peuvent avoir différents états (RUNNING, STOPPED, DETACHED, KILLED, DONE), et ces états sont gérés en fonction des signaux reçus et des actions de l'utilisateur.
* Signaux : Les signaux tels que SIGTERM, SIGSTOP, SIGCHLD, etc., sont utilisés pour contrôler les jobs.

