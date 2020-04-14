
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd, fd2, file_fd, file_fd2;
    int status, i;

    char *find_args[] = {"find", ".", "-type", "f", "-ls", NULL};
    char *cut_args[] = {"cut", "-c", "2-", NULL};
    char *sort_args[] = {"sort", "-n", "-k", "7", NULL};
    char *less_args[] = {"less", NULL};

    /* Creation of the two named pipes */
    char *myfifo = "/tmp/myfifo";
    char *myfifo2 = "/tmp/myfifo2";

    mkfifo(myfifo, 0666);
    mkfifo(myfifo2, 0666);

    pid_t cpid;

    file_fd = open("file.txt", O_CREAT | O_TRUNC | O_WRONLY);
    if (file_fd < 0)
    {
        perror("open");
        exit(1);
    }

    cpid = fork(); /* Creates child 1*/
    if (cpid == 0)
    {
        /* CHILD 1 */
        fd = open(myfifo, O_WRONLY); /* Opens the first named pipe in writing mode */
        dup2(fd, STDOUT_FILENO);     /* Duplicates the new socket file descriptor to the STDOUT file descriptor table*/

        close(fd); /* Close the named pipe */

        execvp(find_args[0], find_args); /* Execution of the find . -type f -ls */
    }
    else
    {
        /* PARENT */
        cpid = fork(); /* Creates child 2 */
        if (cpid == 0)
        {
            /* CHILD 2 */
            fd = open(myfifo, O_RDONLY);   /* Opens the first named pipe in reading mode */
            fd2 = open(myfifo2, O_WRONLY); /* Opens the second named pipe in writing mode */

            dup2(fd, STDIN_FILENO);   /* Duplicates the first named pipe to the STDIN file descriptor table */
            dup2(fd2, STDOUT_FILENO); /* Duplicates the second named pipe to the STDOUT file descriptor table */

            close(fd);  /* Closes the  first named pipe */
            close(fd2); /* Closes the second named pipe */

            execvp(cut_args[0], cut_args); /* Execution of the cut -c 2- */
        }
        else
        {
            /* PARENT */
            cpid = fork(); /* Creates the child 3 */
            if (cpid == 0)
            {
                /* CHILD 3 */
                fd2 = open(myfifo2, O_RDONLY); /* Opens the second named pipe in writing mode */
                dup2(fd2, STDIN_FILENO);       /* Duplicates the second named pipe to the STDIN file descriptor table */

                /* With this dup2 the result get printed to the "myfile.txt" */
                dup2(file_fd, STDOUT_FILENO); /* Duplicates the "myfile.txt" file descriptor to the STDOUT file descriptor table */

                close(fd2); /* Closes the second named pipe */
                close(file_fd);
                execvp(sort_args[0], sort_args); /* Execution of the sort -n -k 7 */
            }
        }
    }

    /* PARENT */
    close(fd);      /* Closes the first named pipe */
    close(fd2);     /* Closes the second named pipe */
    close(file_fd); /* Closes the text file */

    for (i = 0; i < 3; i++)
        wait(&status); /* Waits for the 3 child process to terminate */

    file_fd2 = open("file.txt", O_RDONLY); /* Open the "myfile.txt" read only mode */

    if (file_fd2 < 0)
    {
        perror("file2");
        exit(1);
    }

    dup2(file_fd2, STDIN_FILENO); /* IO redirection */

    execvp(less_args[0], less_args); /* Execution less */

    close(file_fd2); /* Closes the file */

    return 0;
}