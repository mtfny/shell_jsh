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

enum STATE{
    Running, Stopped, Detached, Killed, Done
};

struct job
{
    int num;
    pid_t pid;
    enum STATE etat;
    char command [50];
};
