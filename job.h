#ifndef JOB_H
#define JOB_H
#include <sys/types.h>

enum STATE{
RUNNING, STOPPED, DETACHED, KILLED, DONE
};

typedef struct 
{
    int num;
    pid_t pid;
    enum STATE etat;
    char commande [50];
}job;


void printJob(const job *j);
void concatenate_strings(char **strings, char *result);
void init_job(job *new_job, int num, pid_t pid, char **command);

typedef struct {
    job current_job;
    struct job_list_node *next;
} job_node;

typedef struct {
    int size;
    job_node *head;
} job_list;

void init_job_list(job_list *list);
void print_job_list(job_list *list);
void add_job_to_list(job_list *list, const job *new_job);
int job_get_size(job_list *jobs);
int print_job_int(job_list *jobs, int job);


#endif