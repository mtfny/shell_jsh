#include <stdio.h>
#include <stdlib.h>
#include<stdint.h>
#include <string.h>
#include<dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "commandes.h"

#define MAX_PROMPT_LENGTH 30
#define MAX_PATH_LENGTH 512

void prompt(const char *path){
    char prompt[MAX_PROMPT_LENGTH];

    // On vérifie la longueur
    if (strlen(path) > (MAX_PROMPT_LENGTH - 4)) {
             snprintf(prompt, sizeof(prompt), "... %s ", path + strlen(path) - (MAX_PROMPT_LENGTH - 7)); 
    }else{
         snprintf(prompt, sizeof(prompt), "%s ", path);
    } 

    // Ajoute le prompt à l'historique
    add_history(prompt);

    // Affiche le prompt
    fprintf(stderr, "%s", prompt);
}

int main(int argc, char const *argv[])
{
    rl_outstream = stderr;
    char *line ;

    char *chemin_courant = getcwd(NULL,0);

    while (1)
    {
        prompt(chemin_courant);
        line = readline("$");
        
        if (!line) {
            // L'utilisateur a appuyé sur Ctrl-D pour quitter
            printf("\n");
            break;
        }

        // Ajout de la ligne à l'historique
        add_history(line);

        // traitement de l'instruction donnée par l'utilisateur
        

        // Libération de la mémoire allouée par readline
        free(line);
       
    }


    free(chemin_courant);
  
    return 0;
}



void job_update(){}