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

    for (int i = 0; i < 10; i++) /* Fills the table with i+1 */
        table[i] = i + 1;

    if ((file_fd = open("filetable.bin", O_WRONLY | O_CREAT | O_TRUNC, 0)) == -1) /* Opens the file in write only mode, truncate and creates if it does not exist*/
    {
        perror("Error opening file...");
        exit(1);
    }

    printf("Writing to 'filetable.bin'...\n");
    for (int i = 0; i < 10; i++) /* Write the table to the file in binary mode */
    {
        write(file_fd, &table[i], sizeof(int)); /* Writes 4 bytes at a time */
    }

    close(file_fd); /* Closes the file */

    if (fork() == 0) /* Creates the child process */
    {
        /* CHILD */
        parent_pid = getppid(); /* Gets the parent PID before it dies */
        printf("%d \n", parent_pid);

        if ((file_fd2 = open("filetable.bin", O_RDONLY, 0)) == -1) /* Opens the file in read only mode */
        {
            perror("Error opening file...");
            exit(1);
        }

        printf("Child process reading the file...\n");

        for (int i = 0; i < 10; i++)
        {
            read(file_fd2, &table2[i], sizeof(int)); /* Reads the file 4 bytes at a time and stores in a new table */
        }

        close(file_fd2); /* Closes the file */

        for (int i = 0; i < 10; i++)
        {
            printf("%d ", table2[i]); /* Prints the table to STDOUT */
        }
        printf("\n");

        printf("%d \n", getppid()); /*DEBUG */

        if (getppid() != parent_pid) /* If the PPID is different from the one before it means that the parent is dead */
        {
            printf("Terminating...\n");
            exit(0); /* Terminates */
        }
        else /* If the PPID is still the same it means that the parent is still alive */
        {
            do
            {
                printf("Waiting for parent to die... \n");

            } while (getppid() == parent_pid); /* Waits for the parent to die */

            printf("My parent is finally dead... Terminating!\n");
            exit(0); /* Terminates */
        }
    }
    else
    {
        /* PARENT */
        printf("The parent process is terminating...\n");
        exit(0); /* Terminates */
    }
}
