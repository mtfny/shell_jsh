#include "commandes.h"
#include "redirection.h"
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
#include "job.h"
#include <signal.h>




char** splitString(char* String, int* numWords) {
    char** words = (char**)malloc(1024 * sizeof(char*));

    if (words == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    char* sep = strtok(String, " ");
    *numWords = 0;

    while (sep != NULL) {
        words[*numWords] = (char*)malloc((strlen(sep) + 1) * sizeof(char));

        if (words[*numWords] == NULL) {
            perror("Erreur lors de l'allocation de mémoire");
            exit(EXIT_FAILURE);
        }

        strcpy(words[*numWords], sep);
        (*numWords)++;

        if (*numWords >= 1024) {
            fprintf(stderr, "Trop long\n");
            exit(EXIT_FAILURE);
        }

        sep = strtok(NULL, " ");
    }

    words[*numWords] = NULL;

    return words;
}

void liberer_mots(char **mots,int taille) {
    for (int i = 0; i < taille; i++) {
        free(mots[i]);
    }
}

commande parseCommand(const char *instruction){
    commande cmd = {0};
    char *temp = strdup(instruction); // Copie temporaire pour le parsing
    char *part;
    char *nextPart;

    // Initialisation des valeurs par défaut
    cmd.appendOutput = 0;
    cmd.overwriteOutput = 0;
    cmd.appendError = 0;
    cmd.overwriteError = 0;
    cmd.redir = 0;

    // Analyse de la commande et des redirections
    part = strtok(temp, " ");
    while (part != NULL) {
        nextPart = strtok(NULL, " "); 

        if (strcmp(part, "<") == 0 && nextPart != NULL) {
            cmd.inputFile = strdup(nextPart);
            cmd.redir = 0;
            part = strtok(NULL, " "); // on passe le nom du fichier
        } else if (strcmp(part, ">") == 0 && nextPart != NULL) {
            cmd.outputFile = strdup(nextPart);
            cmd.overwriteOutput = 1;
            cmd.redir = 1;
            part = strtok(NULL, " "); // on passe le nom du fichier
        }else if (strcmp(part, ">|") == 0 && nextPart != NULL) {
            cmd.outputFile = strdup(nextPart);
            cmd.appendOutput = 1;
            cmd.redir = 2;
            part = strtok(NULL, " "); // on passe le nom du fichier
        }else if (strcmp(part, ">>") == 0 && nextPart != NULL) {
            cmd.outputFile = strdup(nextPart);
            cmd.appendOutput = 1;
            cmd.redir = 3;
            part = strtok(NULL, " "); // on passe le nom du fichier
        } else if (strcmp(part, "2>") == 0 && nextPart != NULL) {
            cmd.errorFile = strdup(nextPart);
            cmd.overwriteError = 1;
            cmd.redir = 4;
            part = strtok(NULL, " "); // on passe le nom du fichier
        } else if (strcmp(part, "2>>") == 0 && nextPart != NULL) {
            cmd.errorFile = strdup(nextPart);
            cmd.appendError = 1;
            cmd.redir = 5;
            part = strtok(NULL, " "); // on passe le nom du fichier
        }else if (strcmp(part, "2>|") == 0 && nextPart != NULL) {
            cmd.errorFile = strdup(nextPart);
            cmd.appendError = 1;
            cmd.redir = 6;
            part = strtok(NULL, " "); // on passe le nom du fichier
        }else {
            // Partie de la commande principale
            if (!cmd.cmd) {
                cmd.cmd = strdup(part);
            } else {
                // Concaténation des mots composant la commande
                char *newCmd = malloc(strlen(cmd.cmd) + strlen(part) + 2);
                sprintf(newCmd, "%s %s", cmd.cmd, part);
                free(cmd.cmd);
                cmd.cmd = newCmd;
            }
        }

        part = nextPart; 
    }
   // printf("%s",cmd.cmd);
    free(temp); // Libérer la copie temporaire
    return cmd;
}

void freeCommande(commande *cmd) {
    free(cmd->inputFile);
    free(cmd->outputFile);
    free(cmd->errorFile);
    free(cmd->cmd);
}

/*c'est cette fonction qui va appeler la bonne commande */
int appel(const char *instruction){
    int res = 1;
    char* dupInstruction = strdup(instruction);  

    if (dupInstruction == NULL) {
        perror("Allocation memory error");
        exit(EXIT_FAILURE);
        //return 0;
    }

    if (strcmp(instruction, "") == 0)
    {
        free(dupInstruction);
        return 0;
    }
    
    int numWords;

    char **words = splitString(dupInstruction, &numWords);
   /* if (!isRedirection(instruction))  tentative pour les redirections mais n'a pas fonctionné
    {
        printf("oui c'est bon je vais parse\n");
        commande parsed = parseCommand(instruction);
       // printf("c'est parse\n go executer\n");
        executeRedir(&parsed);


    }else{*/
    if (words[0] == NULL) // aucune instruction n'a été donnée -> l'utilisateur a juste appuyé sur entrer
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
        my_exit(numWords , words);
        res = 1;
    } 

    else if (strcmp(words[0],"jobs") == 0){
        res = cmd_jobs(numWords , words);
    } 
    
    else{ // si le nom de commande ne correspond à aucune commande interne du shell on essaye avec les commandes externes
        res = cmd_externe(numWords,words);
    }
   //}


    free(dupInstruction);
    liberer_mots(words,numWords);

    char buffer[20];  //pour pouvoir stocker la valeur de retour
    // convertion l'entier en chaîne de caractères
    sprintf(buffer, "%d", res);
    int c = setenv("?",buffer,1); //On stock la valeur dans une variable d'environnement
    
    return res;
} 

int pwd(int argc, char *argv[]){
    if (argc > 1){
        fprintf(stderr,"pwd\n");
        return 1;
    }
    char *cwd;

    cwd = getenv("PWD");

    if (cwd!= NULL)
    {
        printf("%s\n",cwd);
        return 0;
    }

    free(cwd);
    return 1;
    
} 


int isDirecrory(char const *chemin) {

    struct stat file_info;//Pour pouvoir stocker les informations sur le fichier

    //On recupère les informations sur le fichier passé en paramètre
    if (lstat(chemin, &file_info) == 0) {
        //On vérifie que ce soit bien un répertoire
        if (S_ISDIR(file_info.st_mode)) {
            return 0;
        } else {
            return 1;
        }
    } else {
        //le fichier n'existe pas
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
            //free(tmp);
            printf("cd : Chemin non valide \n");
            return 1;
        }


        if(chdir(destination) != 0 ){ //On ne peut pas ouvrir le fichier
            printf("cd : Vous ne pouvez pas ouvrir le fichier\n");
            free(destination);
            //free(tmp);
            return 1;
        }

        //On mets à jour les variables d'environnement
        int a = setenv("OLDPWD", tmp, 1);
        int b = setenv("PWD", getcwd(NULL, 0), 1);

        //On libère la mémoire
        free(destination);
        //free(tmp);
        return 0;
    } 


}

int interogation (int argc, char *argv[]){
    if(argc > 1) {
        printf("? : Trop d'arguments donnés en paramètre\n");
    }

    //Si la dernière valeur de retour n'est pas definie, on a rien à renvoyer 
    if(getenv("?") == NULL){ 
        printf("? : Aucune commande récente\n");
        return 1;
    }
    
    printf("%d\n", atoi(getenv("?")));
    return 0;

}

void my_exit(int argc, char *argv[]){
    if(argc > 2){
        perror("exit : Trop d'arguments donnés en paramètre\n");
        return;
    }
    if(job_get_size()>0)
    {
        const char *msg = "Un job est en cours ou suspendu\n";
        write(2, msg, strlen(msg));
        return;
    }

    if(argc == 2){
        //printf("exit avec la valeur %d\n", atoi(argv[1]));

        char buffer[20];  
        snprintf(buffer, sizeof(buffer), "%d", atoi(argv[1]));
        int c =setenv("?",buffer,1); //on change la dernière valeur de retour avec celle fourni

        exit(atoi(argv[1]));
    }else {//S'il n'y a pas d'argument
        if (getenv("?") != NULL){ //Si la dernière valeur de retour est definie, on la renvoie
            //printf("exit avec la valeur %d\n", atoi(getenv("?")));
            exit(atoi(getenv("?")));
        }else{//Sinon on renvoie 0
            //printf("exit avec la valeur 0\n");
            exit(0);
        }
    }
}

void init_jobs()
{
    init_job_list();
}

int cmd_externe(int argc, char *argv[]){
 //       signal(SIGCHLD, sigchld_handler);

    int ret = 0;
    //Création d'un nouveau tableau avec NULL à la fin pour qu'il puisse correspondre à execvp
    char **new_argv = (char **)malloc((argc + 1) * sizeof(char *));
    if (new_argv == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        return 1;
    }

    int arriere_plan = 1;
    if (strcmp(argv[argc-1], "&") == 0){
        arriere_plan = 0;
        argc --;
    }

    //Copie des arguments dans le nouveau tableau
    for (int i = 0; i < argc; i++) {
        new_argv[i] = argv[i];
    }
    new_argv[argc] = NULL;  //Ajouter NULL à la fin du tableau
    
    //On fait un fork pour qu'execvp ne prenne pas la place du père
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("Erreur lors de la création du processus fils");
        free(new_argv);  
        return 1;
    }

    if (child_pid == 0) {
        //On execute la commande externe avec les arguments fournis
        if (execvp(new_argv[0], new_argv) == -1) { //on verifie qu'il n'y a pas eu d'echec
            ret = 1;
        //Si echec, on identifie l'erreur pour fournir un message plus détaillé
            switch (errno)
            {
            case EINVAL:
                perror("Arguments invalides");
                break;
            case ENOENT:
                perror("Commande inexistante");
                break;
            default:
             perror("jsh");
                break;
            }
            exit(EXIT_FAILURE);//On termine
        } else{
            ret = 0;
        }
        exit(EXIT_SUCCESS);
    } else {
        // Code du processus parent
        // On attend que le processus fils (execvp) se termine
        int status;        
        if (waitpid(child_pid, &status, (arriere_plan == 0 ? WNOHANG : 0) | WUNTRACED) == -1) {

            perror("Erreur lors de l'attente du processus fils");
            free(new_argv);  // Libérer la mémoire en cas d'erreur
            return 1;
        }

        //le processus est lancé a l'arrière plan 
        if (arriere_plan == 0){
            pid_t pgid = getpgid(child_pid);
            job job_en_cours;
            init_job(&job_en_cours,job_get_size()+1, child_pid, new_argv);
            add_job_to_jobs(&job_en_cours);
        }

        // Libérer la mémoire du tableau d'arguments
        free(new_argv);
        //On vérifie que le processus s'est terminé correctement
        
        //On retoure valeur fourni par le processus fils (la commande effectuée)
        if (WIFEXITED(status)) return WEXITSTATUS(status) ;
        else ret= 1;
        
    
    
    }

    return ret;//normalement pas atteint mais sinon on a un warning

}

int cmd_jobs(int argc, char *argv[])
{
    if (argc == 1) 
    {
        print_jobs();
        return 0;
    }
    else if  (argc == 2 && strncmp(argv[1], "%", 1) == 0)
    {
        char *num_job = (char *)malloc(strlen(argv[1]));
        strcpy(num_job, argv[1] + 1);
        int ret = print_job_int(atoi(num_job)); 
        free(num_job);
        return ret;
    }
    else return 1;
}

void update()
{
    job_update();

     

    //afficher jobs_done 
    //vider jobs_done
}



int cmd_jobs_size()
{
    return job_get_size();
}