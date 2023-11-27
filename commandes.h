#ifndef COMMANDES_H
#define COMMANDES_H
struct commandes
{
    char commande[30];
};
int appel(const char *chemin_courant, const char *instruction );
char **decoupe_ligne(const char *ligne);
void liberer_mots(char **mots) ;
int pwd();

#endif