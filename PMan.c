/*

Process management shell for Linux.

*/

/* Built in C libraries. */
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

/* Custom C libraries. */
#include "process.h"

/* Initialize function prototypes. */
void bgupdate(process_t* lstProcess);
void bg(char** argv, process_t* lstProcess);
void bglist(process_t* lstProcess);
void bgkill(char** argv, process_t* lstProcess);
void bgstop(char** argv, process_t* lstProcess);
void bgstart(char** argv, process_t* lstProcess, int frombgkill);
void pstat(char** argv, process_t* lstProcess);

/* Update background processes. */
void bgupdate(process_t* lstProcess)
{
	int status;
	
    while(1) 
	{
		int pid = waitpid(-1, &status, WNOHANG);
		if (pid <= 0)
		{
			break;
		}
		else 
		{
			if (remove_by_value(&lstProcess, pid) != -1)
			{
				printf("Process %d terminated.\n", pid);
			}
		}
    }
}

/* Starts a background process. */
void bg(char** argv, process_t* lstProcess)
{
	int status;
	pid_t pid = fork();

	if (pid < 0)
	{
		printf("Failed to create a new process.\n");
	}
	else if (pid == 0)
	{
		if (execvp(argv[0], argv) < 0)
		{
			printf("Error on execvp.\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		/* Get the current working directory and path of the command. */
		char* command = argv[0];
		char* path = "";
		char cwd[1024];

		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			path = malloc(sizeof(cwd)+sizeof(command));
			sprintf(path, "%s %s", cwd, command);
		}

		push(lstProcess, pid, path);
		int retval = waitpid(pid, &status, WNOHANG);
		if (retval == -1)
		{
			printf("Error on waitpid.\n");
			exit(EXIT_FAILURE);
		}
	}
}

/* Lists the current background processes. */
void bglist(process_t* lstProcess)
{
	bgupdate(lstProcess);
	print_process_list(lstProcess);
}

/* Kills the specified background process. */
void bgkill(char** argv, process_t* lstProcess)
{
	pid_t pid = (pid_t)atoi(argv[0]);
	
	if (pid > 0)
	{
		/* Need to start the process again if it is stopped. */
		bgstart(argv, lstProcess, 1);
		if (kill(pid, SIGTERM) == -1)
		{
			printf("Error: Process %d does not exist.\n", pid);
		}
		else
		{
			printf("Process %d terminated.\n", pid);
			remove_by_value(&lstProcess, pid);
		}
	}
}

/* Temporarily stops the specified background process. */
void bgstop(char** argv, process_t* lstProcess)
{
	pid_t pid = (pid_t)atoi(argv[0]);
	
	if (pid > 0)
	{
		if (kill(pid, SIGSTOP) == -1)
		{
			printf("Error: Process %d does not exist.\n", pid);
		}
	}
}

/* Restarts a specified background process. */
void bgstart(char** argv, process_t* lstProcess, int frombgkill)
{
	pid_t pid = (pid_t)atoi(argv[0]);
	
	if (pid > 0)
	{
		if (kill(pid, SIGCONT) == -1 && frombgkill == 0)
		{
			printf("Error: Process %d does not exist.\n", pid);
		}
	}
}

/* Lists the statistics for a specified background process. */
void pstat(char** argv, process_t* lstProcess)
{
	/* Open the stat file for the process. */
	char statFilePath[sizeof(argv[0])+11];
	char statFile[512];
	sprintf(statFilePath, "/proc/%s/stat", argv[0]);
	FILE* statfp = fopen(statFilePath, "r");	
	
	if (statfp == NULL)
	{
		printf("Error: Process %s does not exist.\n", argv[0]);
		return;
	}

	if (fgets(statFile, 512, statfp) != NULL)
	{
		/* Copy the stat file. */
	    char statFileCopy[sizeof(statFile)];
	    strcpy(statFileCopy, statFile);

	    /* Parse into an array of values. */
	    int statCount = 0;
	    char* statToken;

	    statToken = strtok(statFile, " ");
	    while (statToken != NULL)
	    {
	    	statToken = strtok(NULL, " ");
	    	statCount++;
		}

		char* stats[statCount];
		statToken = strtok(statFileCopy, " ");
	    for (int j = 0; j < statCount; j++)
	    {
	    	stats[j] = statToken;
	    	statToken = strtok(NULL, " ");
	    }

	    /* Parse utime and stime. */
	    int utime = atoi(stats[13]) / sysconf(_SC_CLK_TCK);
	    int stime = atoi(stats[14]) / sysconf(_SC_CLK_TCK);

	    /* Print the required values and format the strings. */
		printf("comm: \t\t\t\t%s\n", stats[1]);
		printf("state: \t\t\t\t%s\n", stats[2]);
		printf("utime: \t\t\t\t%d\n", utime);
		printf("stime: \t\t\t\t%d\n", stime);
		printf("rss: \t\t\t\t%s\n", stats[23]);
	}

	fclose(statfp);

	/* Open the status file for the process. */
	char statusFilePath[sizeof(argv[0])+12];
	char statusFile[512];
	sprintf(statusFilePath, "/proc/%s/status", argv[0]);
	FILE* statusfp = fopen(statusFilePath, "r");

	if (statusfp == NULL)
	{
		printf("Error: Process %s does not exist.", argv[0]);
		return;
	}

	/* Get the correct line and print context switches. */
	while (fgets(statusFile, 512, statusfp))
	{
		if (strncmp(statusFile, "voluntary_ctxt_switches", strlen("voluntary_ctxt_switches")) == 0)
		{
			printf("%s", statusFile);
		}

		if (strncmp(statusFile, "nonvoluntary_ctxt_switches", strlen("nonvoluntary_ctxt_switches")) == 0)
		{
			printf("%s", statusFile);
		}
	}
}

/* Prompts the user for input. */
int main()
{
	char* prompt = "PMan: > ";
    char* input = "";
    char* command = "";
    process_t* lstProcess = malloc(sizeof(process_t));

    while (1)
    {
    	/* Check for status updates in background processes. */
		bgupdate(lstProcess);

    	/* Record the input of the user. */
	    input = readline(prompt);
	    char inputCopy[sizeof(input)];
	    strcpy(inputCopy, input);

	    /* Store the command name. */
	    command = strtok(input, " ");
	    if (command == NULL) continue;

	    /* Store the arguments. */
	    int argCount = 0;
	    while (strtok(NULL, " ") != NULL)
	    {
	    	argCount++;
		}

		/* Initialize argv array and skip the first token of the copied input. */
		char* argv[argCount+1];
		strtok(inputCopy, " ");
	    for (int j = 0; j < argCount; j++)
	    {
	    	argv[j] = strtok(NULL, " ");
	    }
	    argv[argCount] = NULL;

	    /* Run the command inputted by the user. */
	    if (strcmp(command, "bg") == 0)
	    {
	    	bg(argv, lstProcess);
	    }
	    else if (strcmp(command, "bglist") == 0)
	    {
	    	bglist(lstProcess);
	    }
	    else if (strcmp(command, "bgkill") == 0)
	    {
	    	if (argCount != 1)
	    	{
	    		printf("%s%s: Invalid arguments.\n", prompt, command);
	    		continue;
	    	}

	    	bgkill(argv, lstProcess);
	    }
	    else if (strcmp(command, "bgstop") == 0)
	    {
	    	if (argCount != 1)
	    	{
	    		printf("%s%s: Invalid arguments.\n", prompt, command);
	    		continue;
	    	}

	    	bgstop(argv, lstProcess);
	    }
	    else if (strcmp(command, "bgstart") == 0)
	    {
	    	if (argCount != 1)
	    	{
	    		printf("%s%s: Invalid arguments.\n", prompt, command);
	    		continue;
	    	}

	    	bgstart(argv, lstProcess, 0);
	    }
	    else if (strcmp(command, "pstat") == 0)
	    {
	    	if (argCount != 1)
	    	{
	    		printf("%s%s: Invalid arguments.\n", prompt, command);
	    		continue;
	    	}

	    	pstat(argv, lstProcess);
	    }
	    else
	    {
	    	printf("%s%s: Command not accepted.\n", prompt, command);
	    }
	}
}