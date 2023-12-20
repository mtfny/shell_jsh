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
    printf("[%d] %d %s %s\n", j->num, j->pid, print_etat, j->commande);


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
}

typedef struct {
    job current_job;
    struct job_node *next;
} job_node;

typedef struct {
    int size;
    job_node *head;
} job_list;


void init_job_list(job_list *list) {
    list->size =0;
    list->head = NULL;
}

void print_job_list(job_list *list) {
    job_node *current = list->head;

    while (current != NULL) {
        printJob(&(current->current_job));
        current = current->next;
    }
}

void add_job_to_list(job_list *jobs, const job *new_job) {
    job_node *new_node = (job_node *)malloc(sizeof(job_node));
    if (new_node == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    memcpy(&(new_node->current_job), new_job, sizeof(job));
    new_node->next = NULL;

    if (jobs->head == NULL) {
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
    print_job_list(jobs);
}





