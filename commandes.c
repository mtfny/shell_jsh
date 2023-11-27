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

struct commandes
{
    char commande[30];
};
/*
c'est cette fonction qui va appeler la bonne commande si l'argument n'est pas bon on enverra un message d'erreur*/
char *decoupe_ligne(const char *ligne){ 
    char **mots = malloc(256 * sizeof(char *));
    if (mots == NULL) {
        perror("Erreur d'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    char *token;
    int i = 0;

    // Utilisation de strtok pour découper la ligne de commande en mots
    char **ligne_copy = strdup(ligne); // Copier la ligne pour éviter de modifier la ligne d'origine
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

/*int appel_commande_speciale(){
 int res;


 return res;
}*/

void appel(const char *chemin,const char *instruction){
    int res = 0;

    char cmd[256]; /* on prevoit jusqu'à 3 arguments (option arg1 arg2)*/
    char arg1[256];
    char arg2[256];
    char arg3[256];

    sscanf(instruction,"%s %s %s",cmd , arg1,arg2);


    //return res;
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



int main(int argc, char const *argv[])
{
    const char *exemple_ligne = "commande1 argument1 argument2";
    char **mots = decoupe_ligne(exemple_ligne);

    // Exemple d'utilisation des mots découpés
    for (int i = 0; mots[i] != NULL; i++) {
        printf("Mot %d : %s\n", i, mots[i]);
    }

    pwd();
    
    return 0;
}
