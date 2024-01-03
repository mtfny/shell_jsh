#ifndef REDIRECTION_H
#define REDIRECTION_H
#include "commandes.h"



int appelRedirection(int *argc, char ***argv);
int containsExactSubstring(const char *c1, const char *c2);


/*Fonctions de redirections*/
int redirectInStandard(const char *cmd);
int redirectOutStandard(const char *cmd);
int redirectOutEcrase(const char *cmd);
int redirectOutConcat(const char *cmd);
int redirectErrStandard(const char *cmd);
int redirectErrEcrase(const char *cmd);
int redirectErrConcat(const char *cmd);
int redirectInPipe(int pipefd[2], char *cmd);
int redirectPipe(char *cmd1, char *cmd2);
#endif
