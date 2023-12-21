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
#include <string.h>
#include <signal.h>


static int val_retour = 0;

enum STATE{
    RUNNING, STOPPED, DETACHED, KILLED, DONE
};

typedef struct 
{
    int num;
    pid_t pid;
    enum STATE etat;
    char commande[255];
}job;

typedef struct {
    job current_job;
    struct job_node *next;
} job_node;

typedef struct {
    int size;
    job_node *head;
} job_list;

static job_list jobs;
static job_list jobs_done;

void printJob(const job *j) {
    const char *print_etat;
    switch (j->etat)
    {
    case RUNNING:
        print_etat = "RUNNING";
        break;
    case STOPPED:
        print_etat = "STOPPED";
        break;
    case DETACHED:
        print_etat = "DETACHED";
        break;
    case KILLED:
        print_etat = "KILLED";
        break;
    case DONE:
        print_etat = "DONE";
        break;

    }

    char output[255]; // Buffer de sortie
    int len = snprintf(output, sizeof(output), "[%d] %d %s %s\n", j->num, j->pid, print_etat, j->commande);
    
    if (len < 0) {
        perror("Erreur lors de la construction de la chaîne de sortie");
        return;
    }
    
    // Écriture du contenu dans stderr
    if (write(STDERR_FILENO, output, len) < 0) {
        perror("Erreur lors de l'écriture sur stderr");
    }

} 

void concatenate_strings(char **strings, char *result) {
    // Initialiser la chaîne résultante avec une chaîne vide
    result[0] = '\0';

    // Concaténer les chaînes individuelles dans le tableau de caractères avec des espaces entre elles
    for (int i = 0; strings[i] != NULL; ++i) {
        strcat(result, strings[i]);
        if (strings[i + 1] != NULL) {
            strcat(result, " ");
        }
    }
}

void init_job(job *new_job, int num, pid_t pid, char **command ) { 
    new_job->num = num;
    new_job->pid = pid;
    new_job->etat = RUNNING;  

    concatenate_strings(command, new_job->commande);
    printJob(new_job);
}

void print_job_list(job_list *list) {
    job_node *current = list->head;

    while (current != NULL) {
        printJob(&(current->current_job));
        current = current->next;
    }
}

void print_jobs()
{
    print_job_list(&jobs);
}

void add_job_to_list(job_list *jobs, const job *new_job) {
    job_node *new_node = (job_node *)malloc(sizeof(job_node));
    if (new_node == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Copie directe de la structure job
    memcpy(&(new_node->current_job), new_job, sizeof(job));

    new_node->next = NULL;

    if (jobs->size == 0) {
        // Si la liste est vide, le nouveau noeud devient la tête de la liste
        jobs->head = new_node;
    } else {
        // Sinon, parcourir la liste jusqu'à la fin et ajouter le nouveau nœud
        job_node *current = jobs->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    // Actualiser la taille
    jobs->size++;
    //test 
}

void add_to_jobs_done(pid_t pid)
{
    job_node *current = jobs.head;

    while (current != NULL)
    {
        if(current->current_job.pid == pid) 
        {
            current->current_job.etat = DONE;
               
            add_job_to_list(&jobs_done, &(current->current_job));
            jobs.size--;
        }
        
        current = current->next;
    }
}

void sigchld_handler(int signum) 
{
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        //printf("Le processus enfant avec PID %d s'est terminé.\n", pid);

        // Ici, vous pouvez mettre à jour votre liste de jobs
        // Par exemple, marquer le job correspondant au PID comme terminé

        //Ajouté à job_done le job de pid "pid"
        add_to_jobs_done(pid);
    }
    if (WIFEXITED(status)) val_retour = WEXITSTATUS(status) ;
    else val_retour = 1;
}

void add_job_to_jobs(const job *new_job)
{
    add_job_to_list(&jobs, new_job);
    
    
}

int job_get_size()
{
    return jobs.size;
}

int print_job_int(int job)
{
    if(jobs.size == 0 || job == 0) return 1;
    else
    {
        job_node *current = jobs.head;

        while (current != NULL)
        {
            if(current->current_job.num == job) 
            {
                printJob(&(current->current_job));
                return 0;
            }
            current = current->next;
        }
        return 1; 
    }
}

void job_update()
{
   print_job_list(&jobs_done);
   jobs_done.size = 0;
   jobs_done.head = NULL;

}

void init_job_list() {
    jobs.size =0;
    jobs.head = NULL;

    jobs_done.size =0;
    jobs_done.head = NULL;

    signal(SIGCHLD, sigchld_handler);
}

int get_val_retour()
{
    return val_retour;    
}





