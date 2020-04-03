/** 
 * C program that emulates the following shell command line:
 * * find . -type f -ls | cut -c 2- | sort -n -k 7 >file.txt; less <file.txt
 *   The first pipeline is implemented using a unix type socket
 *   The second pipeline is implemented using a pipe
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

int main(void)
{
    int pipe_fd[2], file_fd, file_fd2;
    int status, i, fromlen;
    register int s, len, ns;
    char *find_args[] = {"find", ".", "-type", "f", "-ls", NULL};
    char *cut_args[] = {"cut", "-c", "2-", NULL};
    char *sort_args[] = {"sort", "-n", "-k", "7", NULL};
    char *less_args[] ={"less",NULL};
    struct sockaddr_un saun, fsaun;

    pid_t cpid;

    //Criacao do pipe
    pipe(pipe_fd);

    file_fd = open("file.txt", O_CREAT | O_TRUNC | O_WRONLY);
    if (file_fd < 0)
    {
        perror("open");
        exit(1);
    }

    cpid = fork();
    if (cpid == 0)
    { /* child */
        if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            perror("server: socket");
            exit(1);
        }

        saun.sun_family = AF_UNIX;
        strcpy(saun.sun_path, ADDRESS);

        unlink(ADDRESS);
        len = sizeof(saun.sun_family) + strlen(saun.sun_path);

        if (bind(s, &saun, len) < 0)
        {
            perror("server: bind");
            exit(1);
        }

        if (listen(s, 5) < 0)
        {
            perror("server: listen");
            exit(1);
        }

        if ((ns = accept(s, &fsaun, &fromlen)) < 0)
        {
            perror("server: accept");
            exit(1);
        }

        dup2(ns, STDOUT_FILENO);

        close(s);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        execvp(find_args[0], find_args);
    }
    else
    {
        cpid = fork();
        if (cpid == 0)
        {
            if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
            {
                perror("client: socket");
                exit(1);
            }

            saun.sun_family = AF_UNIX;
            strcpy(saun.sun_path, ADDRESS);

            len = sizeof(saun.sun_family) + strlen(saun.sun_path);

            while (connect(s, &saun, len) < 0)
            {
                //perror("client: connect");
            }

            dup2(s, STDIN_FILENO);
            dup2(pipe_fd[1], STDOUT_FILENO);

            close(s);
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            execvp(cut_args[0], cut_args);
        }
        else
        {
            cpid = fork();
            if (cpid == 0)
            {

                dup2(pipe_fd[0], STDIN_FILENO);
                dup2(file_fd, STDOUT_FILENO);

                close(pipe_fd[0]);
                close(pipe_fd[1]);

                execvp(sort_args[0], sort_args);
            }
        }
    }

    close(s);
    close(file_fd);
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    for (i = 0; i < 3; i++)
        wait(&status);

    file_fd2=open("file.txt", O_RDONLY);

    if (file_fd2 < 0)
    {
        perror("file2");
        exit(1);
    }

    dup2(file_fd2,STDIN_FILENO);

    execvp(less_args[0], less_args);

    close(file_fd2);

    return 0;
}