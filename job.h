#ifndef JOB_H
#define JOB_H

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

#endif