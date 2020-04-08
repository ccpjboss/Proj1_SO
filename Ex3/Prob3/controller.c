#include <signal.h>    /*Signals*/
#include <sys/types.h> /*Process*/
#include <unistd.h>    /*Process*/
#include <stdlib.h>
#include <stdio.h> /*Files*/

void handler(int signum); /* Signal handler function */
pid_t pid;

int main(int argc, char const *argv[])
{
    struct sigaction sig;
    sig.sa_handler = handler; /* Handle function to run when a signal is received */

    FILE *agentptr;

    if ((agentptr = fopen("agent.pid", "r")) == NULL) /* Open in reading mode */
    {
        printf("File could not be opened!\n");
        perror("AGENT.PID\n");
        exit(1);
    }
    else
    {
        fscanf(agentptr, "%d", &pid); /* reads the file and stores the PID in global var pid */
        printf("PID: %d\n", pid);
        fclose(agentptr); /* fclose closes the file */
    }

    if (kill(pid, 0) == -1) /* Checks for the existence of the agent sending a signal 0 that returns -1 on error */
    {
        printf("Error: no agent found...\n");
        perror("NO AGENT!\n");
        exit(1);
    }
    else
    {
        printf("Agent found with PID: %d \n", pid);
    }

    if (sigaction(SIGINT, &sig, NULL) == -1) /* Associate SIGINT with sigaction struct a*/
        perror("SIGACTION--SIGINT");

    int opc;

    while (1) /* Infinite loop to stay in the menu until SIGINT is received */
    {
        printf("Choose a signal to send [1: HUP; 15: TERM]: \n");
        scanf("%d", &opc); /* Option input */

        switch (opc)
        {
        case 1:
            printf("Sending a SIGHUP signal to agent...\n");
            if (kill(pid, 1) == -1) /* Send a SIGHUP to the agent */
            {
                perror("SIGHUP \n");
                exit(1);
            }

            break;

        case 15:
            printf("Sending a SIGTERM signal to agent...\n");
            if (kill(pid, 15) == -1) /* Send a SIGTERM to the agent */
            {
                perror("SIGTERM \n");
                exit(1);
            }
            printf("Terminating...\n");
            exit(0);
            break;

        default: /* Checks for wrong input */
            printf("Option not valid...\n");
            break;
        }
    }

    return 0;
}

void handler(int signum)
{
    if (signum == SIGINT) /* If SIGINT is received */
    {
        printf("\nSIGINT received...\n");
        if (kill(pid, 15) == -1) /* Send a SIGTERM to the agent*/
        {
            perror("Sending SIGTERM \n");
            exit(1);
        }
        printf("Terminating...\n");
        exit(0); /* normal exit status */
    }
}
