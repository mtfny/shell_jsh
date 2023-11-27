#include "commandes.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdint.h>
#include<dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>



/*découpe la ligne en tableau de mots*/
char **decoupe_ligne(const char *ligne){ 
    char **mots = malloc(256 * sizeof(char *));
    if (mots == NULL) {
        perror("Erreur d'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    char *token;
    int i = 0;

    // Utilisation de strtok pour découper la ligne de commande en mots
    char *ligne_copy = strdup(ligne); // Copier la ligne pour éviter de modifier la ligne d'origine
    if (ligne_copy == NULL) {
        perror("Erreur d'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    token = strtok(ligne_copy, " ");
    while (token != NULL) {
        mots[i] = strdup(token); // Allouer de l'espace pour chaque mot
        if (mots[i] == NULL) {
            perror("Erreur d'allocation de mémoire");
            exit(EXIT_FAILURE);
        }
        i++;
        token = strtok(NULL, " ");
    }
    mots[i] = NULL; // La liste de mots est terminée par NULL

    free(ligne_copy); // Libérer la mémoire de la copie temporaire de la ligne

    return mots;
}

void liberer_mots(char **mots) {
    for (int i = 0; mots[i] != NULL; i++) {
        free(mots[i]);
    }
    free(mots);
}

/*int appel_commande_speciale(){  [À FAIRE]
 int res;


 return res;
}*/

/*c'est cette fonction qui va appeler la bonne commande */
int appel(const char *chemin,const char *instruction){
    int res;
    char **mots = decoupe_ligne(instruction);

    if (mots[0] == NULL) // aucune instruction n'a été donnée
    {
        res = 1;
    }
    

    char *cmd = mots[0] ; /* on prevoit jusqu'à 3 arguments après la commande (option arg1 arg2)*/
    char *arg1 = mots[1];
    char *arg2 = mots[2];
    char *arg3 = mots[3];

   
    if (strcmp(cmd,"pwd") == 0)
    {
        if (arg1 != NULL || arg2 != NULL || arg3 != NULL) // pwd doit etre appelée sans arguments
        {
            res = 1;
            perror("erreur : pwd avec arguments");
            return res;
        }else{
             res = pwd();
        }
       
    }

    return res;
} 

int pwd(){
    char *cwd;
    char buff [PATH_MAX + 1];

    cwd = getcwd(buff,PATH_MAX+1);

    if (cwd != NULL)
    {
        printf("%s\n",cwd);
        return 0;
    }

    perror("getcwd pour pwd");
    return 1;
    
}


/*

int pwd(const char *chemin_courant){}

int ls(char *PATH){}

int *cd(char *PATH){}

int cp(char *fic1,*har *fic2){}

int mv(char *fic1,char *fic2){}
*/


