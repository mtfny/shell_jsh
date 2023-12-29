#include "redirection.h"
#include "commandes.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


int containsExactSubstring(const char *c1, const char *c2){
  // Diviser c1 en mots
    char *token;
    char c1Copy[1024]; 
    strcpy(c1Copy, c1);

    token = strtok(c1Copy, " ");
    while (token != NULL) {
        // Comparer chaque mot avec c2
        if (strcmp(token,c2) == 0) {
            return 1;  // Mot trouvé, c2 est une sous-chaîne exacte de c1
        }
        token = strtok(NULL, " ");
    }

    return 0;  // Aucun mot de c2 trouvé dans c1
}

void truncate_argv(int *argc, char ***argv) {
   int i;
    for (i = 0; i < *argc; i++) {
        if (strcmp((*argv)[i], ">") == 0 || strcmp((*argv)[i], "<") == 0 ||
            strcmp((*argv)[i], ">>") == 0 || strcmp((*argv)[i], "2>") == 0 ||
            strcmp((*argv)[i], "2>>") == 0 || strcmp((*argv)[i], ">|") == 0
            || strcmp((*argv)[i], "2>|") == 0)
             {
            break;
            }
    }

    // Mettre à jour argc pour refléter le nombre d'éléments avant le symbole de redirection
    *argc = i;

    // Créer un nouveau tableau argv avec la taille ajustée
    char **new_argv = malloc(sizeof(char *) * (*argc + 1));
    if (new_argv == NULL) {
        perror("Erreur lors de l'allocation de mémoire pour new_argv");
        return;
    }

    // Copier les éléments de l'ancien argv dans le nouveau
    for (int j = 0; j < *argc; j++) {
        new_argv[j] = (*argv)[j];
    }

    // S'assurer que le dernier élément est NULL
    new_argv[*argc] = NULL;

    // Libérer la mémoire de l'ancien argv si nécessaire
    // ...

    // Mettre à jour le pointeur argv pour qu'il pointe vers le nouveau tableau
    *argv = new_argv;    
}




int appelRedirection(int *argc, char ***argv){
    int res = 0;
    for (int i = 0; i < *argc; ++i) {
        if (containsExactSubstring((*argv)[i],"<")) {
            res = redirectInStandard((*argv)[i + 1]);
            truncate_argv(argc,argv);
        }
        else if (containsExactSubstring((*argv)[i],">"))
        {
           res = redirectOutStandard((*argv)[i+1]);
            truncate_argv(argc,argv);
        } else if ( containsExactSubstring((*argv)[i],">>"))
        {
           res = redirectOutConcat((*argv)[i+1]);
            truncate_argv(argc,argv);
        }else if (containsExactSubstring((*argv)[i],">|"))
        {
           res = redirectOutEcrase((*argv)[i+1]);
            truncate_argv(argc,argv);
        }else if (containsExactSubstring((*argv)[i],"2>"))
        {
           res = redirectErrStandard((*argv)[i+1]);
            truncate_argv(argc,argv);
        }else if ( containsExactSubstring((*argv)[i],"2>>"))
        {
           res = redirectErrConcat((*argv)[i+1]);
            truncate_argv(argc,argv);
        }else if (containsExactSubstring((*argv)[i],"2>|"))
        {
           res = redirectErrEcrase((*argv)[i+1]);
            truncate_argv(argc,argv);
        }
    }

    return res;
}

int redirectInStandard(const char *cmd) { 
        int fd = open(cmd, O_RDONLY);
        if (fd == -1) {
            perror("Erreur lors de l'ouverture du fichier d'entrée");
            return 1; 
        }

        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("Erreur lors de la redirection de l'entrée standard");
            close(fd);
            return 1; 
        }

        close(fd);
    

    return 0; 
}

int redirectOutStandard(const char *cmd) {
        int fd;
        fd = open(cmd, O_WRONLY | O_CREAT | O_EXCL, 0644);

        if (fd == -1) {
            perror("Erreur lors de l'ouverture du fichier de sortie");
            return 1;
           // exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Erreur lors de la redirection de la sortie standard");
            close(fd);
            return 1;
        }

        close(fd);
    return 0;
}

int redirectOutEcrase(const char *cmd) {


        int fd;
        // Ouvrir le fichier avec écrasement éventuel
        fd = open(cmd, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1) {
            perror("Erreur lors de l'ouverture du fichier de sortie");
            return 1;
        }

        // Rediriger la sortie standard vers le fichier
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Erreur lors de la redirection de la sortie standard");
            close(fd);
            return 1;
        }

        close(fd);

    return 0;
}

int redirectOutConcat(const char *cmd){
 
        int fd;
        fd = open(cmd, O_WRONLY | O_CREAT | O_APPEND, 0644);

        if (fd == -1) {
            perror("Erreur lors de l'ouverture du fichier de sortie");
            return 1;
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Erreur lors de la redirection de la sortie standard");
            close(fd);
            return 1;
        }

        close(fd);
    
    return 0;
}

int redirectErrStandard(const char *cmd){
        int fd = open(cmd, O_WRONLY | O_CREAT | O_EXCL, 0644); // Créer si n'existe pas, échouer si existe
    if (fd == -1) { 
         perror("Erreur lors de l'ouverture du fichier de sortie");
        return 1;
    }
    if(dup2(fd, STDERR_FILENO) == -1){
         perror("Erreur lors de la redirection de la sortie standard");
            close(fd);
            return 1;
    } 
    close(fd); 
    return 0;
}

int redirectErrEcrase(const char *cmd){

        int fd = open(cmd,O_WRONLY | O_CREAT | O_TRUNC, 0644); // Créer si n'existe pas, échouer si existe
    if (fd == -1) { 
         perror("Erreur lors de l'ouverture du fichier de sortie");
        return 1;
    }
    if(dup2(fd, STDERR_FILENO) == -1){
         perror("Erreur lors de la redirection de la sortie standard");
            close(fd);
            return 1;
    } 
    close(fd); 
    return 0;
}
int redirectErrConcat(const char *cmd){
   
        int fd = open(cmd,O_WRONLY | O_CREAT | O_APPEND, 0644); // Créer si n'existe pas, échouer si existe
    if (fd == -1) { 
         perror("Erreur lors de l'ouverture du fichier de sortie");
        return 1;
    }
    if(dup2(fd, STDERR_FILENO) == -1){
         perror("Erreur lors de la redirection de la sortie standard");
            close(fd);
            return 1;
    } 
    close(fd); 
    
    return 0;
}
int redirectErrPipe(int pipefd[2], char *cmd){
    
    int fd = open(cmd, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Créer si n'existe pas, échouer si existe
    if (fd == -1) { 
         perror("Erreur lors de l'ouverture du fichier de sortie");
        return 1;
    }
    if(dup2(fd, STDERR_FILENO) == -1){
         perror("Erreur lors de la redirection de la sortie standard");
            close(fd);
            return 1;
    } 
    close(fd); 
    
    return 0;
}

