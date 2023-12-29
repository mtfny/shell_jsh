#ifndef JOB_H
#define JOB_H
#include <sys/types.h>

enum STATE{
RUNNING, STOPPED, DETACHED, KILLED, DONE
};

typedef struct job
{
    int num;
    pid_t pid;
    enum STATE etat;
    char commande [50];
    int print_while_done;
}job;

typedef struct job_node{
    job current_job;
   struct job_node *next;
} job_node;

typedef struct job_list{
    int size;
    job_node *head;
} job_list;


void printJob(job *j);
void concatenate_strings(char **strings, char *result);
void init_job(job *new_job, int num, pid_t pid, char **command);


void init_job_list();
void print_job_list(job_list *list);
void print_jobs();
void add_job_to_list(job_list *jobs, const job *new_job);
void add_job_to_jobs(const job *new_job);
void add_job_to_list_bis(const job *new_job);
int job_get_size();
int print_job_int(int job);
void job_update();
void remove_done_killed_jobs();
void add_to_jobs_done(pid_t pid);
int kill_pid(int sig, pid_t pid);
int kill_job(int sig, int job);


#endif