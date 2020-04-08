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

    char buf[255]; /*Buffer to print output of file*/

    struct sigaction sig;
    sig.sa_handler = handler; /* Handle function to run when a signal is received */

    FILE *agentptr; /*Pointer to "agent.pid" file*/

    if ((agentptr = fopen("agent.pid", "w")) == NULL) /*Open the file in writing mode*/
    {
        printf("File could not be opened!\n");
        perror("AGENT.PID\n");
        exit(1);
    }
    else
    {
        printf("Printing PID to 'agent.pid' file...\n");
        fprintf(agentptr, "%d", myPID); /* Prints his PID to the file*/
        fclose(agentptr);               /* Closes the file */
    }

    FILE *textptr; /*Pointer to text.in file*/

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

        while (fgets(buf, 255, textptr)) /* Gets the content of the file and stores it in a buffer */
        {
            printf("%s", buf); /* Prints the buffer */
        }
        printf("-----------------------------\n");
        fclose(textptr); /* Closes the file */
    }

    while (1) /* Infinite loop to check for signals */
    {
        if (sigaction(SIGHUP, &sig, NULL) == -1) /* Associate SIGHUP with sigaction struct sig*/
            perror("SIGACTION--SIGHUP");

        if (sigaction(SIGTERM, &sig, NULL) == -1)/* Associate SIGTERM with sigaction struct sig*/
            perror("SIGACTION--SIGTERM");

        pause(); 
    }

    return 0;
}

void handler(int signum)
{
    if (signum == SIGHUP) /* If the signal received is a SIGHUP read the file*/
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

    if (signum == SIGTERM) /* If the signal received is a SIGTERM then just terminate */
    {
        printf("Process terminating...\n");
        exit(0); /*Normal exit status*/
    }
}