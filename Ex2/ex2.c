/** 
 * C program that emulates the following shell command line:
 * find . -type f -ls | cut -c 2- | sort -n -k 7 >file.txt; less <file.txt
 * The first pipeline is implemented using a unix type socket;
 * The second pipeline is implemented using a named pipe;
 */

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

#define ADDRESS "mysocket"

int main(int argc, char *argv[])
{
    int fifo_fd, file_fd, file_fd2;
    int status, i, fromlen;
    register int s, len, ns;

    char *find_args[] = {"find", ".", "-type", "f", "-ls", NULL};
    char *cut_args[] = {"cut", "-c", "2-", NULL};
    char *sort_args[] = {"sort", "-n", "-k", "7", NULL};
    char *less_args[] = {"less", NULL};

    struct sockaddr_un local, remote; /* Socket names */

    pid_t cpid;

    char *myfifo = "/tmp/myfifo"; /* Path to named pipe */

    mkfifo(myfifo, 0666); /* Creation of the named pipe with permissions */

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
        if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) /* Socket creation */
        {
            perror("server: socket");
            exit(1);
        }

        local.sun_family = AF_UNIX;      /* Make the socket type UNIX */
        strcpy(local.sun_path, ADDRESS); /* Copies ADDRESS to local.sun_path */
        unlink(local.sun_path);

        len = sizeof(local.sun_family) + strlen(local.sun_path); /* Length of local address to bind to socket */

        if (bind(s, (struct sockaddr *)&local, len) < 0) /* Binds the socket to local address */
        {
            perror("server: bind");
            exit(1);
        }

        if (listen(s, 5) < 0) /* Prepares to accept incoming connections */
        {
            perror("server: listen");
            exit(1);
        }

        fromlen = sizeof(remote);

        if ((ns = accept(s, (struct sockaddr *)&remote, &fromlen)) < 0) /* Accepts the connections */
        {
            perror("server: accept");
            exit(1);
        }

        dup2(ns, STDOUT_FILENO); /* Duplicates the new socket file descriptor to the STDOUT file descriptor table*/

        close(s);                        /* Close the socket*/
        close(ns);                       /* Close the new socket */
        execvp(find_args[0], find_args); /* Execution of the find . -type f -ls */
    }
    else
    {
        /* PARENT */
        cpid = fork(); /* Creates child 2 */
        if (cpid == 0)
        {
            /* CHILD 2 */
            fifo_fd = open(myfifo, O_WRONLY); /* Opens the named pipe in writing mode to write the result of the shell command */

            if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) /* Socket creation */
            {
                perror("client: socket");
                exit(1);
            }

            remote.sun_family = AF_UNIX;      /* Make the socket type UNIX */
            strcpy(remote.sun_path, ADDRESS); /* Copies ADDRESS to remote.sun_path */

            len = sizeof(remote.sun_family) + strlen(remote.sun_path); /* Length of local address to bind to socket */

            while (connect(s, (struct sockaddr *)&remote, len) < 0)
                ; /* Connects to the remote address */

            dup2(s, STDIN_FILENO);        /* Duplicates the socket file descriptor to the STDIN file descriptor table */
            dup2(fifo_fd, STDOUT_FILENO); /* Duplicates the named pipe file descriptor to the STDOUT file descriptor table */

            close(s);       /* Closes the socket */
            close(fifo_fd); /*Closes the named pipe */

            execvp(cut_args[0], cut_args); /* Execution of the cut -c 2- */
        }
        else
        {
            /* PARENT */
            cpid = fork(); /* Creates the child 3 */
            if (cpid == 0)
            {
                /* CHILD 3 */
                fifo_fd = open(myfifo, O_RDONLY);
                dup2(fifo_fd, STDIN_FILENO); /* Duplicates the pipe (reading end) file descriptor to the STDIN file descriptor table */

                /* With this dup2 the result get printed to the "myfile.txt" */
                dup2(file_fd, STDOUT_FILENO); /* Duplicates the "myfile.txt" file descriptor to the STDOUT file descriptor table */

                close(fifo_fd); /* Closes the named pipe */
                close(file_fd); /* Closes the file */
                execvp(sort_args[0], sort_args); /* Execution of the sort -n -k 7 */
            }
        }
    }

    /* PARENT */
    close(s);       /* Closes the socket */
    close(file_fd); /* Closes the file */
    close(fifo_fd); /* Closes the named pipe */

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

/**
 * Explanation of the shell command: find . -type f -ls | cut -c 2- | sort -n -k 7 >file.txt; less <file.txt
 * The command above consists of 3 shell commands all pipelined. The output of the command get printed
 * to a text file and then that file gets printed to the STDOUT via the less shell command.
 * 
 * find . -type f -ls --> Find all the files of type 'regular file' and lists in ls -dils format on standard output.
 * cut -c 2- --> Cuts each line of the input from thse previous command. In this case cuts the first 1 character, which is a space.
 * sort -n -k 7 >file.txt --> Sorts line of texts, this case a nomeric sort (-n) sorting by the size of the file which is the 7th column 
 * 
 * Then the file write to the file.
 * 
 * Finally it prints the file to the terminal using less editor.
 */