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
    int pipeIndex = 0;
    if (new_argv == NULL) {
        perror("Erreur lors de l'allocation de mémoire pour new_argv");
        return 1;
    }

    for (int i = 0; i < *argc; ++i) {
        if (containsExactSubstring((*argv)[i], "|")) {
           // printf("\n oui j'ai trouvé un pipe à la pos %d\n",i);
            pipeIndex = i;
            break;
        }
    }

    if (pipeIndex > 0) {
        // Créer les deux parties pour le pipe
         // Calculez la longueur nécessaire pour cmd1 et cmd2
        int lengthCmd1 = 0, lengthCmd2 = 0;
        for (int i = 0; i < pipeIndex; ++i) {
            lengthCmd1 += strlen((*argv)[i]) + 1; // +1 pour l'espace ou le caractère nul
        }
        for (int i = pipeIndex + 1; i < *argc; ++i) {
            lengthCmd2 += strlen((*argv)[i]) + 1;
        }

        char *cmd1 = malloc(lengthCmd1 * sizeof(char));
        char *cmd2 = malloc(lengthCmd2 * sizeof(char));
        if (cmd1 == NULL || cmd2 == NULL) {
            perror("Erreur lors de l'allocation de mémoire");
            exit(EXIT_FAILURE);
        }

        cmd1[0] = '\0';
        cmd2[0] = '\0';
        
        for (int i = 0; i < pipeIndex; ++i) {
            strncat(cmd1, (*argv)[i], lengthCmd1 - strlen(cmd1) - 1);
            strncat(cmd1, " ", lengthCmd1 - strlen(cmd1) - 1);
        }
        for (int i = pipeIndex + 1; i < *argc; ++i) {
             strncat(cmd2, (*argv)[i], lengthCmd2 - strlen(cmd2) - 1);
             strncat(cmd2, " ", lengthCmd2 - strlen(cmd2) - 1);
        }
     
        res = redirectPipe(cmd1, cmd2); //fonction pour exécuter les commandes avec un pipe

        free(cmd1);
        free(cmd2);
    
    } else {
        for (int i = 0; i < *argc; i++)
        {
            if(containsExactSubstring((*argv)[i],"<")) { //si on tombe sur un symbole de redirection alors
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
int redirectPipe(char *cmd1, char *cmd2) {
    int pipefd[2];
    pid_t cpid1, cpid2;
     int status1, status2;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    cpid1 = fork();
    if (cpid1 == -1) {
        perror("fork");
        close(pipefd[0]); 
        close(pipefd[1]);
        return 1;
    }

    if (cpid1 == 0) { // Premier processus enfant (cmd1)
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
         exit(appel(cmd1) != 1 ? EXIT_SUCCESS : EXIT_FAILURE);
    } else {
        cpid2 = fork();
        if (cpid2 == -1) {
            perror("fork");
            close(pipefd[0]);
            close(pipefd[1]);
            return 1;
        }
        if (cpid2 == 0) { // Deuxième processus enfant (cmd2)
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            exit(appel(cmd2) != 1 ? EXIT_SUCCESS : EXIT_FAILURE);
        } else {
            // Processus parent 
            close(pipefd[0]);
            close(pipefd[1]);

            waitpid(cpid1, &status1, 0); 
            waitpid(cpid2, &status2, 0); 

          
       if (WIFEXITED(status1) && WEXITSTATUS(status1) != 0) {
            return WEXITSTATUS(status1); // Retourne le code de sortie de cpid1 s'il a échoué
        }

        if (WIFEXITED(status2)) {
            return WEXITSTATUS(status2); // Retourne le code de sortie de cpid2
        }

        }
    }
    return 0; // En cas de succès
}
