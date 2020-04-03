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
    char buf[255];

    FILE *agentptr; /*Pointer to "agent.pid" file*/
    if ((agentptr = fopen("agent.pid", "w")) == NULL)
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

    FILE *textptr;
    if ((textptr = fopen("text.in", "r")) == NULL)
    {
        printf("Couldn't open the file.\n");
        perror("TEXT.IN");
        exit(1);
    }
    else
    {
        printf("Reading the 'text.in' file...\n");
        printf("-----------------------------\n");
        /* while not end of file */
        while (fgets(buf, 255, textptr))
        {
            printf("%s", buf);
        }
        printf("-----------------------------\n");
        fclose(textptr);
    }

    return 0;
}