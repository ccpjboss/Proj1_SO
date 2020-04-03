/**
 * ! AGENT
 * 1->The agent program creates a "agent.pid" file and writes its PID to it.
 * TODO: 2->The agent then read the contents of a file nammed "text.in" and prints it.
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

int main(int argc, char const *argv[])
{
    pid_t myPID;
    myPID = getpid();

    FILE *agentptr; /*Pointer to "agent.pid" file*/
    if ((agentptr = fopen("agent.pid", "w")) == NULL)
    {
        printf("File could not be oppened!\n");
        exit(1);
    }
    else
    {
        printf("Printing PID to 'agent.pid' file...\n");
        fprintf(agentptr, "%d", myPID);
        fclose(agentptr);
    }

    while (1)
    {
    }

    return 0;
}