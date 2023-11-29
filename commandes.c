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
}

/*int appel_commande_speciale(){  [À FAIRE]
 int res;


 return res;
}*/

/*c'est cette fonction qui va appeler la bonne commande */
int appel(const char *instruction){
    int res = 1;
    char* inputString = strdup(instruction);  // Utilisation de strdup pour allouer dynamiquement de la mémoire

    if (inputString == NULL) {
        perror("Allocation memory error");
        exit(EXIT_FAILURE);
    }

    if (strcmp(instruction, "") == 0)
    {
        free(inputString);
        return 0;
    }
    
    int numWords;

    char **words = splitString(inputString, &numWords);

    if (words[0] == NULL) // aucune instruction n'a été donnée
    {
        res = 0;
    }

    else if (strcmp(words[0],"pwd") == 0)
    {
        res = pwd(numWords ,words);  
    }

    else if (strcmp(words[0],"cd") == 0){
        res = cd(numWords , words);
    }

    else if (strcmp(words[0],"?") == 0){
        res = interogation(numWords , words);
    } 

    else if (strcmp(words[0],"exit") == 0){
        res = my_exit(numWords , words);
    } 
    
    else{ // si le nom de commande ne correspond à aucune commande interne du shell on essaye avec les commandes externes
        res = cmd_externe(numWords,words);
    }

    //if (strcmp(words[0],"ls") == 0){
       // printf("test d'ls\n");
       // res = ls(numWords,words);
    
    
    

    free(inputString);
    liberer_mots(words);

    char buffer[20];  // Choisissez une taille suffisamment grande pour contenir la représentation de l'entier
    // Utilisation de sprintf pour convertir l'entier en chaîne de caractères
    sprintf(buffer, "%d", res);
    int c =setenv("?",buffer,1);
    
    return res;
} 

int pwd(int argc, char *argv[]){
    if (argc > 1){
        printf("pwd : Trop d'arguments donnés en paramètre\n");
        return 1;
    }
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
            return 0;
        } else {
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
        printf ("cd : Trop d'arguments donnés en paramètre\n");
        return 1;
    } else{
        char *destination = malloc(1024);

        if (argc == 1 ){ // La commande cd sans argument -> retour à la racine 
            if (getenv("HOME") == NULL){ // Pas de variable HOME définie 
                printf("cd : Pas de valeur : HOME");
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
            printf("cd : Chemin non valide \n");
            return 1;
        }


        if(chdir(destination) != 0 ){ //On ne peut pas ouvrir le fichier
            printf("cd : Vous ne pouvez pas ouvrir le fichier\n");
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

int interogation (int argc, char *argv[]){
    if(argc > 1) {
        printf("? : Trop d'arguments donnés en paramètre\n");
    }

    if(getenv("?") == NULL){ 
        printf("? : Aucune commande récente\n");
        return 1;
    }
    
    printf("%d\n", atoi(getenv("?")));
    return 0;

}

int my_exit(int argc, char *argv[]){
    if(argc > 2){
        printf("exit : Trop d'arguments donnés en paramètre\n");
        return 1;
    }

    if(argc == 2){
        printf("exit avec la valeur %d\n", atoi(argv[1]));

        char buffer[20];  
        snprintf(buffer, sizeof(buffer), "%d", atoi(argv[1]));
        int c =setenv("?",buffer,1);
        
        printf("dfdfdf %d \n", c);
        exit(atoi(argv[1]));
        return(atoi(argv[1]));
    }else {
        if (getenv("?") != NULL){
            printf("exit avec la valeur %d\n", atoi(getenv("?")));
            exit(atoi(getenv("?")));
        }else{
            printf("exit avec la valeur 0\n");
            exit(0);
        }
    }

    return 0;
}



int cmd_externe(int argc, char *argv[]){
    // Créer un nouveau tableau avec NULL à la fin pour qu'il puisse correspondre à execvp
    char **new_argv = (char **)malloc((argc + 1) * sizeof(char *));
    if (new_argv == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        return 1;
    }

    // Copier les arguments dans le nouveau tableau
    for (int i = 0; i < argc; i++) {
        new_argv[i] = argv[i];
    }
    new_argv[argc] = NULL;  // Ajouter NULL à la fin du tableau

    // Créer un processus fils
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("Erreur lors de la création du processus fils");
        free(new_argv);  // Libérer la mémoire en cas d'erreur
        return 1;
    }

    if (child_pid == 0) {
        // Code du processus fils

        // Exécuter la commande externe avec les arguments fournis
        if (execvp(new_argv[0], new_argv) == -1) {
            // Afficher un message d'erreur plus informatif
           
            switch (errno)
            {
            case EINVAL:
                perror("arguments invalides");
                break;
            case ENOENT:
                perror("commande inexistante");
                break;
            default:
             perror("jsh");
                break;
            }
            // Terminer le processus fils en cas d'erreur
            free(new_argv);  // Libérer la mémoire en cas d'erreur
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else {
        // Code du processus parent

        // Attendre que le processus fils se termine
        int status;
        if (waitpid(child_pid, &status, 0) == -1) {
            perror("Erreur lors de l'attente du processus fils");
            free(new_argv);  // Libérer la mémoire en cas d'erreur
            return 1;
        }

        // Libérer la mémoire du tableau d'arguments
        free(new_argv);

        // Vérifier le statut de sortie du processus fils
        if (WIFEXITED(status)) {
            // Le processus fils s'est terminé normalement
            return WEXITSTATUS(status) ;
        } else {
            return 1;
        }
    }

    return 1;//normalement pas atteint mais sinon on a un warning

}

