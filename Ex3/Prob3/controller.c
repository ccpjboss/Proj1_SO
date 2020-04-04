/**
 * ! AGENT
 * * 1->The agent program creates a "agent.pid" file and writes its PID to it.
 * * 2->The agent then read the contents of a file nammed "text.in" nad prints it.
 * ! Sensitive to SIGHUP and SIGTERM
 * * SIGHUP -> reads the contents of "text.in".
 * * SIGTERM -> Process terminates... and exit.
 * 
 * ! CONTROLLER
 * TODO: 1-> Checks for runnig agent by fetching the PID from "agent.pid".
 * TODO: 2-> If it cannot find agent --> prints("Error: no agent found.") and exits otherwise prints("Agent found.")
 * TODO: 3-> Signal menu: 
 *                 * Choose a signal to send [1: HUP; 15: TERM]:
 * ! SIGTERM also terminates the controller
 * ! If the user presses Crtl+C the controller should send a SIGTERM to agent and exits
 */

#include <signal.h>    /*Signals*/
#include <sys/types.h> /*Process*/
#include <unistd.h>    /*Process*/
#include <stdlib.h>
#include <stdio.h> /*Files*/

int main(int argc, char const *argv[])
{
    pid_t pid;
    FILE *agentptr;

    if ((agentptr = fopen("agent.pid", "r")) == NULL) /*Open in writing mode*/
    {
        printf("File could not be oppened!\n");
        perror("AGENT.PID\n");
        exit(1);
    }
    else
    {
        fscanf(agentptr, "%d", &pid);
        printf("PID: %d\n", pid);
        fclose(agentptr); /* fclose closes the file */
    }

    if (kill(pid, 0) == -1)
    {
        printf("Error: no agent found...\n");
        perror("NO AGENT!\n");
        exit(1);
    }
    else
    {
        printf("Agent found with PID: %d \n", pid);
    }

    return 0;
}
