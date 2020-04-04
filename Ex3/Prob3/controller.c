/**
 * ! AGENT
 * * 1->The agent program creates a "agent.pid" file and writes its PID to it.
 * * 2->The agent then read the contents of a file nammed "text.in" nad prints it.
 * ! Sensitive to SIGHUP and SIGTERM
 * * SIGHUP -> reads the contents of "text.in".
 * * SIGTERM -> Process terminates... and exit.
 * 
 * ! CONTROLLER
 * 1-> Checks for runnig agent by fetching the PID from "agent.pid".
 * 2-> If it cannot find agent --> prints("Error: no agent found.") and exits otherwise prints("Agent found.")
 * 3-> Signal menu: 
 *                 * Choose a signal to send [1: HUP; 15: TERM]:
 * ! SIGTERM also terminates the controller
 * ! If the user presses Crtl+C the controller should send a SIGTERM to agent and exits
 */

#include <signal.h>    /*Signals*/
#include <sys/types.h> /*Process*/
#include <unistd.h>    /*Process*/
#include <stdlib.h>
#include <stdio.h> /*Files*/

void handler(int signum);
pid_t pid;

int main(int argc, char const *argv[])
{
    int opc;
    FILE *agentptr;
    struct sigaction a;
    a.sa_handler = handler;

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

    if (sigaction(SIGINT, &a, NULL) == -1)
        perror("SIGACTION--SIGINT");

    while (1)
    {
        printf("Choose a signal to send [1: HUP; 15: TERM]: \n");
        scanf("%d", &opc);

        switch (opc)
        {
        case 1:
            printf("Sending a SIGHUP signal to agent...\n");
            if (kill(pid, 1) == -1)
            {
                perror("Sending SIGHUP \n");
                exit(1);
            }

            break;

        case 15:
            printf("Sending a SIGTERM signal to agent...\n");
            if (kill(pid, 15) == -1)
            {
                perror("Sending SIGTERM \n");
                exit(1);
            }
            printf("Terminating...\n");
            exit(0);
            break;

        default:
            printf("Option not valid...\n");
            break;
        }
    }

    return 0;
}

void handler(int signum)
{
    if (signum == SIGINT)
    {
        printf("\nSIGINT received...\n");
        if (kill(pid, 15) == -1)
        {
            perror("Sending SIGTERM \n");
            exit(1);
        }
        printf("Terminating...\n");
        exit(0);
    }
}
