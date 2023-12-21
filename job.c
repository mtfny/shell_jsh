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
        print_etat = "Running";
        break;
    case STOPPED:
        print_etat = "Stopped";
        break;
    case DETACHED:
        print_etat = "Detached";
        break;
    case KILLED:
        print_etat = "Killed";
        break;
    case DONE:
        print_etat = "Done";
        break;

    }

    char output[255]; 
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
        // Sinon, parcourir la liste jusqu'à la fin et ajouter le nouveau noeud
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
void add_job_to_list_bis(const job *new_job) {
    job_node *new_node = (job_node *)malloc(sizeof(job_node));
    if (new_node == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Copie directe de la structure job
    memcpy(&(new_node->current_job), new_job, sizeof(job));

    // Faire pointer le nouveau nœud vers l'ancienne tête de liste
    new_node->next = jobs_done.head;

    // Faire du nouveau nœud la nouvelle tête de liste
    jobs_done.head = new_node;

    // Actualiser la taille
    jobs_done.size++;
}

void add_to_jobs_done()
{
    job_node *current = jobs.head;

    while (current != NULL)
    {
       pid_t pid = current->current_job.pid;
        int status;

        pid_t result = waitpid(pid, &status, WNOHANG);

        if (result == 0) {
            // Le processus enfant n'a pas encore terminé
        } else if (result == pid) {
            // Le processus enfant a terminé
            if (WIFEXITED(status)) {
                // Le processus enfant a terminé normalement
                //printf("azalakapaino\n");
                current->current_job.etat = DONE;
                add_job_to_list_bis( &(current->current_job));
                jobs.size --;
                int exit_status = WEXITSTATUS(status);
                
            }
        } else {
            // Erreur a gerer 
        }
        
        current = current->next;
    }
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
    add_to_jobs_done();
    print_job_list(&jobs_done);
    jobs_done.size = 0;
    jobs_done.head = NULL;

}

void init_job_list() {
    jobs.size =0;
    jobs.head = NULL;

    jobs_done.size =0;
    jobs_done.head = NULL;

    //signal(SIGCHLD, sigchld_handler);
}

int get_val_retour()
{
    return val_retour;    
}

