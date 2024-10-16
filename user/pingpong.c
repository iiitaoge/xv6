#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);

    char readtext[10];//作为父进程和子进程的读出容器

    int pid = fork();

    if (pid == 0)   // 子进程
    {
        read(p[0], readtext, 10);   // 读取
        printf("%d: received %s\n", getpid(), readtext);
        write(p[1], "pong", 10);
        exit(0);
    }
    else
    {
        write(p[1], "ping", 10);
        wait(0);
        read(p[0], readtext, 10);
        printf("%d: received %s\n", getpid(), readtext);
    }
    exit(0);
}