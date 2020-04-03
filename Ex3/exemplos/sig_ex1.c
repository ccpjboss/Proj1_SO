#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sigHandle(int var);
void sigHandle2(int var);

int main()
{
    signal(SIGINT, sigHandle);
    signal(SIGQUIT, sigHandle2);

    while (1)
    {
        sleep(1);
        printf("I ' m running \n ");
    }; /* infinite loop */
    return 0;
}

void sigHandle(int var)
{
    signal(SIGINT, sigHandle);
    
    printf(" You have pressed ctrl - c \n");
}

void sigHandle2(int var)
{
    signal(SIGQUIT, sigHandle2);

    printf("You have pressed Ctrl - \ \n");
    exit(0);
}