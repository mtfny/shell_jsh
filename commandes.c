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



char** splitString(char* inputString, int* numWords) {
    char** words = (char**)malloc(1024 * sizeof(char*));

    if (words == NULL) {
        perror("Allocation memory error");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(inputString, " ");
    *numWords = 0;

    while (token != NULL) {
        words[*numWords] = (char*)malloc((strlen(token) + 1) * sizeof(char));

        if (words[*numWords] == NULL) {
            perror("Allocation memory error");
            exit(EXIT_FAILURE);
        }

        strcpy(words[*numWords], token);
        (*numWords)++;

        if (*numWords >= 1024) {
            fprintf(stderr, "Too many words, increase MAX_WORDS\n");
            exit(EXIT_FAILURE);
        }

        token = strtok(NULL, " ");
    }

    return words;
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
int appel(const char *instruction){
    int res;

    char* inputString = strdup(instruction);  // Utilisation de strdup pour allouer dynamiquement de la mémoire

    if (inputString == NULL) {
        perror("Allocation memory error");
        exit(EXIT_FAILURE);
    }
    
    int numWords;

    char** words = splitString(inputString, &numWords);

    if (words[0] == NULL) // aucune instruction n'a été donnée
    {
        res = 1;
    }

    if (strcmp(words[0],"pwd") == 0)
    {
             res = pwd();  
    }

    if (strcmp(words[0],"cd") == 0){
        res = cd(numWords , words);
    }

    free(inputString);
    free(words);

    return res;
} 

int pwd(){
    char *cwd;
    char buff [PATH_MAX + 1];

    cwd = getenv("PWD");

    if (cwd != NULL)
    {
        printf("%s\n",cwd);
        return 0;
    }

    perror("getcwd pour pwd");
    return 1;
    
} 


int isDirecrory(char const *chemin) {

    struct stat file_info;

    // Utilisation de lstat pour obtenir des informations sur le fichier ou le répertoire
    if (lstat(chemin, &file_info) == 0) {
        // Vérifier si c'est un répertoire
        if (S_ISDIR(file_info.st_mode)) {
            printf("%s est un répertoire.\n", chemin);
            return 0;
        } else {
            printf("%s n'est pas un répertoire.\n", chemin);
            return 1;
        }
    } else {
        // Si lstat échoue, cela peut signifier que le fichier n'existe pas
        return 2;
    }
}


int cd (int argc, char *argv[])
{   
    if(argc > 2){  // La commande cd avec plusieurs arguments -> erreur 
        perror ("Trop d'arguments donnés en paramètre\n");
        return 1;
    } else{
        char *destination = malloc(1024);

        if (argc == 1 ){ // La commande cd sans argument -> retour à la racine 
            if (getenv("HOME") == NULL){ // Pas de variable HOME définie 
                perror("Pas de valeur : HOME");
                free(destination);
                return 1;
            }else{ // destination -> référence de HOME
                strcpy(destination, getenv("HOME"));
            }
        }

        if (argc == 2){ // La commande cd avec un arguments
            if (strcmp(argv[1], "-") == 0 && getenv("OLDPWD") != NULL){ //destination -> dernier répertoire ouvert 
                strcpy(destination, getenv("OLDPWD"));
            }else if (strcmp(argv[1], "~") == 0 && getenv("HOME") != NULL){ //destination -> dernier répertoire ouvert 
                strcpy(destination, getenv("HOME"));
            }else{ //cas normal 
                strcpy(destination, argv[1]);
            }
        }
        char *tmp = getcwd(NULL, 0);


        if (isDirecrory(destination) != 0){ //Si le fichier n'existe pas, ou existe mais n'est pas un répertoire
            free(destination);
            free(tmp);
            perror("Chemin non valide \n");
            return 1;
        }


        if(chdir(destination) != 0 ){ //On ne peut pas ouvrir le fichier
            perror("Vous ne pouvez pas ouvrir le fichier\n");
            free(destination);
            free(tmp);
            return 1;
        }

        //lastRef = strdup(tmp);
        int a = setenv("OLDPWD", tmp, 1);
        

        //ref = getcwd(NULL, 0);
        int b = setenv("PWD", getcwd(NULL, 0), 1);

        free(destination);
        free(tmp);
        return 0;
    }


}


/*

int pwd(const char *chemin_courant){}

int ls(char *PATH){}

int *cd(char *PATH){}

int cp(char *fic1,*har *fic2){}

int mv(char *fic1,char *fic2){}
*/


