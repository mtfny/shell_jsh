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





int appelRedirection(int *argc, char ***argv){
    int res = 0;
    char **new_argv = malloc(sizeof(char *) * (*argc));
    int new_argc = 0;

    if (new_argv == NULL) {
        perror("Erreur lors de l'allocation de mémoire pour new_argv");
        return 1;
    }
    for (int i = 0; i < *argc; ++i) {
        if (containsExactSubstring((*argv)[i],"<")) { //si on tombe sur un symbole de redirection alors
            res = redirectInStandard((*argv)[i + 1]); //on tente d'effectuer la redirection avec le nom du fichier fourni après le symbole
             i++; //on incrémente i pour passer le nom du fichier
        }
        else if (containsExactSubstring((*argv)[i],">"))
        {
           res = redirectOutStandard((*argv)[i+1]);
           i++;
        } else if ( containsExactSubstring((*argv)[i],">>"))
        {
           res = redirectOutConcat((*argv)[i+1]);
           i++;
        }else if (containsExactSubstring((*argv)[i],">|"))
        {
           res = redirectOutEcrase((*argv)[i+1]);
           i++;
        }else if (containsExactSubstring((*argv)[i],"2>"))
        {
           res = redirectErrStandard((*argv)[i+1]);
           i++;
        }else if ( containsExactSubstring((*argv)[i],"2>>"))
        {
           res = redirectErrConcat((*argv)[i+1]);
           i++;
        }else if (containsExactSubstring((*argv)[i],"2>|"))
        {
           res = redirectErrEcrase((*argv)[i+1]);
           i++;
        }else
        {
            new_argv[new_argc++] = (*argv)[i]; //il s'agit d'une commande on va donc la stocker pour pouvoir l'executer ensuite 
        }
        
    }
    new_argv[new_argc] = NULL;
    *argc = new_argc;
    *argv = new_argv;

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

int redirectPipe(char *instructions){
    char *pipePos = strstr(instructions, "|");
    if (pipePos != NULL) {
        *pipePos = '\0'; // Couper l'instruction au pipe
        char *firstPart = strdup(instructions);
        char *secondPart = strdup(pipePos + 1);

        if (!firstPart || !secondPart) {
            perror("strdup");
            free(firstPart);
            free(secondPart);
            return 1;
        }

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            free(firstPart);
            free(secondPart);
            return 1;
        }

        pid_t cpid = fork();
        if (cpid == -1) {
            perror("fork");
            free(firstPart);
            free(secondPart);
            return 1;
        }

        if (cpid == 0) { // Enfant
            close(pipefd[0]); // Ferme la lecture du pipe
            dup2(pipefd[1], STDOUT_FILENO); // Redirige stdout vers l'entrée du pipe
            close(pipefd[1]);

            int res = appel(firstPart);
            exit(res);
        } else { // Parent
            close(pipefd[1]); // Ferme l'écriture du pipe
            dup2(pipefd[0], STDIN_FILENO); // Redirige stdin vers la sortie du pipe
            close(pipefd[0]);

            int res = appel(secondPart);
            waitpid(cpid, NULL, 0); // Attendre la fin de l'enfant

            free(firstPart);
            free(secondPart);
            return res;
        }
    }else{
        return 0;
    }

}