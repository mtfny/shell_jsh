#ifndef COMMANDES_H
#define COMMANDES_H
struct commandes
{
    char commande[30];
};

//fontions auxiliaires
char** splitString(char* inputString, int* numWords);
void liberer_mots(char **mots,int taille) ;

//Fonctions pour les commandes
int appel(const char *instruction );
int pwd(int argc, char *argv[]);
int isDirecrory(char const *chemin);
int cd(int argc, char  *argv[]);
int interogation (int argc, char *argv[]);
void my_exit(int argc, char *argv[]);
void init_jobs();
int cmd_externe(int argc, char *argv[]);
int cmd_jobs(int argc, char *argv[]);
void update();
//void sigchld_handler(int signum);
int cmd_jobs_size();

#endif