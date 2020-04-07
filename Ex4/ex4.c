/**
 * Write 'i+1' intergers into element 'i' of a table MAXBUF=10
 * Write table to file 'filetable.bin', binary format
 * TODO: Create child process and terminate the parent
 * TODO: Child process reads the file in binary format and prints to standard output
 * TODO: Waits for the parent to terminate and terminates
 */

#include <unistd.h>    /*File op*/
#include <sys/types.h> /*Process*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAXBUF 10

int main(int argc, char const *argv[])
{
    int table[MAXBUF];
    int table2[MAXBUF];
    pid_t parent_pid;
    int file_fd;
    int file_fd2;

    printf("Creating table...\n");

    for (int i = 0; i < 10; i++)
        table[i] = i + 1;

    if ((file_fd = open("filetable.bin", O_WRONLY | O_CREAT | O_TRUNC, 0)) == -1)
    {
        perror("Error opening file...");
        exit(1);
    }

    printf("Writing to 'filetable.bin'...\n");
    for (int i = 0; i < 10; i++)
    {
        write(file_fd, &table[i], sizeof(int));
    }

    close(file_fd);

    if (fork() == 0)
    {
        /* CHILD */
        parent_pid = getppid();
        printf("%d \n",parent_pid);
        if ((file_fd2 = open("filetable.bin", O_RDONLY, 0)) == -1)
        {
            perror("Error opening file...");
            exit(1);
        }

        printf("Child process reading the file...\n");
        for (int i = 0; i < 10; i++)
        {
            read(file_fd2, &table2[i], sizeof(int));
        }

        close(file_fd2);

        for (int i = 0; i < 10; i++)
        {
            printf("%d ", table2[i]);
        }
        printf("\n");
        printf("%d \n", getppid());

        if (getppid() != parent_pid)
        {
            printf("Terminating...\n");
            exit(0);
        }
        else
        {
            do{

                printf("Waiting for parent to die... \n");

            }while (getppid()==parent_pid);
            printf("My parent is finally dead... Terminating!\n");
        }
    }
    else
    {
        /* PARENT */
        printf("The parent process is terminating...\n");
        exit(0);
    }
}
