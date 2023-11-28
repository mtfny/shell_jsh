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
#include "job.h"

#define MAX_PROMPT_LENGTH 30
#define MAX_PATH_LENGTH 512

void job_update(){
    /*mettre à jour le nombre de jobs et le tableau*/
}

void prompt(const char *path,int jobs){
    char prompt[MAX_PROMPT_LENGTH];

    int max = MAX_PROMPT_LENGTH -3;

    if (jobs > 9)
    {
        max = MAX_PROMPT_LENGTH - 4;
    }
    

    // On vérifie la longueur
    if (strlen(path) > max) {
             snprintf(prompt, sizeof(prompt), "[%d]...%s ",jobs, path + strlen(path) - (MAX_PROMPT_LENGTH - 7)); 
    }else{
         snprintf(prompt, sizeof(prompt), "[%d] %s ",jobs, path);
    } 

    // Affiche le prompt
    fprintf(stderr, "%s", prompt);
}

int main(int argc, char const *argv[])
{
    rl_outstream = stderr;
    char *line ;

    //char *chemin_courant = getcwd(NULL,0);
    

    int nb_job = 0;
    struct job *tableau_job = malloc(nb_job * sizeof(struct job));
    char *chemin_courant = getenv("PWD");
    

    while (1)
    {
        job_update();
        prompt(chemin_courant,nb_job);
        line = readline("$ ");
        
        if (!line) {
            // L'utilisateur a appuyé sur Ctrl-D pour quitter
            printf("\n");
            break;
        }

        // Ajout de la ligne à l'historique
        add_history(line);

        // traitement de l'instruction donnée par l'utilisateur
            
        appel(line);
            
        chemin_courant = getenv("PWD");
           
        // Libération de la mémoire allouée par readline
        free(line);
       
    }


  
    return 0;
}



