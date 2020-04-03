//find . -type f -ls | cut -c 2- | sort -n -k 7 >file.txt; less <file.txt”

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#define ADDRESS "mysocket"

int main(void)
{
    int pipe_fd[2];
    int status, i, fromlen;
    register int s, len, ns;
    char *find_args[] = {"find", ".", "-type", "f", "-ls", NULL};
    char *cut_args[] = {"cut", "-c", "2-", NULL};
    char *sort_args[] = {"sort", "-n", "-k", "7", NULL};
    struct sockaddr_un saun, fsaun;

    pid_t cpid;

    //Criacao do pipe
    pipe(pipe_fd);

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

            close(s);

            execvp(cut_args[0], cut_args);
        }
    }
    close(s);

    for (i = 0; i < 2; i++)
        wait(&status);
}