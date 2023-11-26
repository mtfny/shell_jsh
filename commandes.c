#include "commandes.h"
#include <stdio.h>
#include <stdlib.h>
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
c'est cette fonction qui va appeler la bonne commande si l'argument n'est pas bon on enverra un message d'erreur
void appel(char commande, char path_exec){

} 




int ls(char PATH){}

int *cd(char PATH){}

int cp(char fic1,char fic2){}

int mv(char fic1,char fic2){}
*/



int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
