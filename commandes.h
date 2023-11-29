#ifndef COMMANDES_H
#define COMMANDES_H
struct commandes
{
    char commande[30];
};
char** splitString(char* inputString, int* numWords);
int appel(const char *instruction );
void liberer_mots(char **mots) ;
int pwd(int argc, char *argv[]);
int isDirecrory(char const *chemin);
int cd(int argc, char  *argv[]);
int interogation (int argc, char *argv[]);
int my_exit(int argc, char *argv[]);
//int ls(int argc, char *argv[]);
int cmd_externe(int argc, char *argv[]);
#endif