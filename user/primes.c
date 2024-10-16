#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define RE 0
#define WR 1

void primes(int *left_fd)
{
    close(left_fd[WR]); // 因为父进程已经 write 完毕， 关闭父进程通过左管道写入子进程的写端
    int prime = 0, i = 0;
    if (read(left_fd[RE], &prime, sizeof(int)) != 0) // 管道还有数据在传输，没数据就代表结束
    {
        printf("prime %d\n", prime); // 第一个是素数，输出

        int right_fd[2];    // 父进程连接子进程的管道
        pipe(right_fd); // 初始化管道

        int pid = fork();
        if (pid == 0)
        {
            primes(right_fd);
        }
        else
        {   
            close(right_fd[RE]);    // 父进程不需要从子进程读数据，关闭
            while (read(left_fd[RE], &i, sizeof(int)) != 0)
            {
                if (i % prime)
                {
                    write(right_fd[WR], &i, sizeof(int));
                }
            }
            close(right_fd[WR]);    // 写入子进程完毕
            close(left_fd[RE]);     // 从父进程读取完毕
            wait(0);
        }   
    }
}

int main(int argc,char * argv[])
{
    int left_fd[2];
    pipe(left_fd);
    int pid = fork();
    if (pid == 0) // 子进程
    {
        primes(left_fd);    // 这里把第一个进程和剩余进程区分开，primes函数是第二个到最后一个进程
    }
    else 
    {
        close(left_fd[RE]);
        for (int i = 2; i <= 35; i++)
        {
            write(left_fd[WR], &i, sizeof(int));
        }
        close(left_fd[WR]);
        wait(0);
    }
    exit(0);
}