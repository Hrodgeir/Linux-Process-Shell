#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "process.h"

void print_process_list(process_t* head)
{
	int jobCount = 0;
    process_t* current = head->next;

	while (current != NULL)
    {
	    printf("%d: %s\n", current->pid, current->path);
	    jobCount++;
	    current = current->next;
    }
    
    printf("Total background jobs: %d\n", jobCount);
}

void push(process_t* head, pid_t pid, char* path)
{
	process_t* current = head;
	while (current->next != NULL)
	{
		current = current->next;
	}

	/* Add a new process. */
	current->next = malloc(sizeof(process_t));
	current->next->pid = pid;
	current->next->path = path;
	current->next->next = NULL;
}

pid_t remove_by_value(process_t** head, pid_t pid) 
{
	pid_t retpid = -1;
    process_t* previous;
    process_t* current;

    if ((*head)->next == NULL)
	{
		return retpid;
	}

    if ((*head)->next->pid == pid)
    {
        process_t* next_process = NULL;

        next_process = (*head)->next->next;
	    retpid = (*head)->next->pid;
	    free((*head)->next);
	    (*head)->next = next_process;

	    return retpid;
    }

    previous = current = (*head)->next;
    while (current)
    {
        if (current->pid == pid) 
        {
            previous->next = current->next;
            free(current);
            return pid;
        }

        previous = current;
        current  = current->next;
    }

    return retpid;
}