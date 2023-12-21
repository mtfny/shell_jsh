#include "redirection.h"
#include "commandes.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

int isRedirection(const char *instruction) {
    //printf("début test redirection\n");
   if (containsExactSubstring(instruction,"<")||
        containsExactSubstring(instruction,">")||
        containsExactSubstring(instruction,">>")||
        containsExactSubstring(instruction,">|")||
        containsExactSubstring(instruction,"2>")||
        containsExactSubstring(instruction,"2>>")||
        containsExactSubstring(instruction,"2>|")
    ) {
            //printf("fin ok\n");
        // La chaîne contient au moins une redirection
        return 0;
    } else {
       // printf("fin pas ok\n");
        // Aucune redirection détectée
        return 1;
    }
}

int containsExactSubstring(const char *c1, const char *c2){
  // Diviser c1 en mots
    char *token;
    char c1Copy[100]; 
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



void redirect(int oldfd, int newfd) {
    if (oldfd != -1) {
        dup2(oldfd, newfd);
        close(oldfd);
    }
}

char* truncate_command(const char* instruction) {
    char* command;
    char* token;
    char delimiters[] = "><|"; 
    // Copier l'instruction dans une nouvelle chaîne pour ne pas la modifier
    command = strdup(instruction);
    if (command == NULL) {
        perror("Erreur lors de la duplication de la chaîne");
        return NULL;
    }

    // Trouver le premier token (mot)
    token = strtok(command, delimiters);
    if (token == NULL) {
        free(command);
        return NULL;
    }

    // Réallouer la mémoire pour ne contenir que la commande
    char* truncated = realloc(command, strlen(token) + 1);
    if (truncated == NULL) {
        perror("Erreur de réallocation de mémoire");
        free(command);
        return NULL;
    }

    return truncated;
}


int executeRedir(const commande *cmd){
   
    int fileDescriptor;
    int res;
    pid_t pid;
  
  
    
     pid = fork();
    if (pid == -1) {
        // Erreur de fork
        perror("Erreur fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // processus enfant

        // Découper la commande en arguments pour execvp
        int argc;
        char *command = truncate_command(cmd->cmd);
        char **argv = splitString(command,&argc);
        // Redirection de l'entrée
          if (cmd->inputFile != NULL) {
        fileDescriptor = open(cmd->inputFile, O_RDONLY);
        if (fileDescriptor == -1) {
            perror("Erreur lors de l'ouverture du fichier d'entrée");
            return 1;
        }
        if (dup2(fileDescriptor, STDIN_FILENO) == -1) {
            perror("Erreur lors de la redirection de l'entrée standard");
            return 1;
        }
        close(fileDescriptor);
    }

    // Redirection de la sortie
    if (cmd->outputFile != NULL) {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->overwriteOutput) {
            flags |= O_TRUNC;
        } else if (cmd->appendOutput) {
            flags |= O_APPEND;
        } else {
            flags |= O_EXCL;
        }

        fileDescriptor = open(cmd->outputFile, flags, 0644);
        if (fileDescriptor == -1) {
            perror("Erreur lors de l'ouverture du fichier de sortie");
            return 1;
        }
        if (dup2(fileDescriptor, STDOUT_FILENO) == -1) {
            perror("Erreur lors de la redirection de la sortie standard");
            return 1;
        }
        close(fileDescriptor);
    }

    // Redirection de l'erreur
    if (cmd->errorFile != NULL) {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->overwriteError) {
            flags |= O_TRUNC;
        } else if (cmd->appendError) {
            flags |= O_APPEND;
        } else {
            flags |= O_EXCL;
        }

        fileDescriptor = open(cmd->errorFile, flags, 0644);
        if (fileDescriptor == -1) {
            perror("Erreur lors de l'ouverture du fichier d'erreur");
            return 1;
        }
        if (dup2(fileDescriptor, STDERR_FILENO) == -1) {
            perror("Erreur lors de la redirection de l'erreur standard");
            return 1;
        }
        close(fileDescriptor);
    }

        // Exécuter la commande
        if (execvp(argv[0], argv) == -1) {
            perror("Erreur execvp");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else {
        // processus parent

        // Attendre que le processus enfant se termine
        int status;
        waitpid(pid, &status, 0);

         if (WIFEXITED(status)) return WEXITSTATUS(status) ;
    }

    return res;

}
