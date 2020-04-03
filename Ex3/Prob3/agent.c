/**
 * ! AGENT
 * 1->The agent program creates a "agent.pid" file and writes its PID to it.
 * TODO: 2->The agent then read the contents of a file named "text.in" and prints it.
 * ! Sensitive to SIGHUP and SIGTERM
 * TODO: SIGHUP -> reads the contents of "text.in".
 * TODO: SIGTERM -> Process terminates... and exit.
 * 
 * ! CONTROLLER
 * * 1-> Checks for runnig agent by fetching the PID from "agent.pid".
 * * 2-> If it cannot find agent --> prints("Error: no agent found.") and exits otherwise prints("Agent found.")
 * * 3-> Signal menu: 
 *                 * Choose a signal to send [1: HUP; 15: TERM]:
 * ! SIGTERM also terminates the controller
 * ! If the user presses Crtl+C the controller should send a SIGTERM to agent and exits
 */

#include <signal.h>    /*Signals*/
#include <stdio.h>     /*Files*/
#include <sys/types.h> /*Process*/
#include <unistd.h>    /*Process*/
#include <stdlib.h>

void handler(int signum);

int main(int argc, char const *argv[])
{
    pid_t myPID;
    myPID = getpid(); /*Gets PID*/
    char buf[255];    /*Buffer to print output of file*/
    struct sigaction a;
    a.sa_handler = handler;

    FILE *agentptr;                                   /*Pointer to "agent.pid" file*/
    if ((agentptr = fopen("agent.pid", "w")) == NULL) /*Open in writing mode*/
    {
        printf("File could not be oppened!\n");
        perror("AGENT.PID\n");
        exit(1);
    }
    else
    {
        printf("Printing PID to 'agent.pid' file...\n");
        fprintf(agentptr, "%d", myPID);
        fclose(agentptr);
    }

    FILE *textptr;                                 /*Pointer to text.in file*/
    if ((textptr = fopen("text.in", "r")) == NULL) /*Open in reading mode*/
    {
        printf("Couldn't open the file.\n");
        perror("TEXT.IN");
        exit(1);
    }
    else
    {
        printf("Reading the 'text.in' file...\n");
        printf("-----------------------------\n");

        while (fgets(buf, 255, textptr))
        {
            printf("%s", buf);
        }
        printf("-----------------------------\n");
        fclose(textptr);
    }

    while (1)
    {
        if (sigaction(SIGHUP, &a, NULL) == -1)
        {
            perror("SIGACTION");
        }

        pause();
    }

    return 0;
}

void handler(int signum)
{
    if (signum == SIGHUP)
    {
        FILE *textptr;
        char buf[255];                                 /*Pointer to text.in file*/
        if ((textptr = fopen("text.in", "r")) == NULL) /*Open in reading mode*/
        {
            printf("Couldn't open the file.\n");
            perror("TEXT.IN");
            exit(1);
        }
        else
        {
            printf("Reading the 'text.in' file...\n");
            printf("-----------------------------\n");

            while (fgets(buf, 255, textptr))
            {
                printf("%s", buf);
            }
            printf("-----------------------------\n");
            fclose(textptr);
        }
    }
}