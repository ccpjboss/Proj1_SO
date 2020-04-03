#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sigHandleChild(int var);
void sigHandleParent(int var);
void sigHandleParent2(int var);

__pid_t cpid;

int main(int argc, char const *argv[])
{

    if ((cpid = fork()) == 0)
    {
        /* CHILD */
        signal(SIGINT, sigHandleChild);
        signal(SIGQUIT, sigHandleChild);

        for (;;)
            ;
    }
    else
    {
        /*PARENT*/
        signal(SIGINT, sigHandleParent);
        signal(SIGQUIT, sigHandleParent2);
        for (;;)
            ;
    }
}

void sigHandleParent(int var)
{
    signal(SIGINT, sigHandleParent);

    kill(cpid, 2);
    kill(cpid, 9);
    wait(NULL);

    cpid = fork();
}

void sigHandleChild(int var)
{
    signal(SIGINT, sigHandleChild);
    signal(SIGQUIT, sigHandleChild);

    printf("I'm the child process. Bye bye.");
}

void sigHandleParent2(int var)
{
    signal(SIGQUIT, sigHandleParent2);

    kill(cpid,2);
    kill(cpid, 9);
    wait(NULL);
    printf("I'm the parent process. Bye bye.");

    exit(0);
}