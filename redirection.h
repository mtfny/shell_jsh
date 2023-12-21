#ifndef REDIRECTION_H
#define REDIRECTION_H
#include "commandes.h"

int isRedirection(const char *instruction);
int appelRedirection(const commande *parsedCommand);
int containsExactSubstring(const char *c1, const char *c2);
int executeRedir(const commande *cmd);

char* truncate_command(const char* instruction);

/*Fonctions de redirections*/
/*int redirectInStandard(const commande *cmd);
int redirectOutStandard(const commande *cmd);
int redirectOutEcrase(const commande *cmd);
int redirectOutConcat(const commande *cmd);
int redirectErrStandard(const commande *cmd);
int redirectErrEcrase(const commande *cmd);
int redirectErrConcat(const commande *cmd);
int redirectInPipe(int pipefd[2], commande *cmd);*/
void redirect(int oldfd, int newfd);

#endif