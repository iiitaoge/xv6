#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char* readline() {
    char* buf = malloc(100);  // 分配100字节的缓冲区用于读取一行输入
    char* p = buf;
    while(read(0, p, 1) != 0){  // 从标准输入（文件描述符0）中读取字符
        if(*p == '\n' || *p == '\0'){  // 如果遇到换行或字符串结束符
            *p = '\0';  // 将换行符或结束符替换为 '\0'，以形成完整的 C 字符串
            return buf;  // 返回读取的字符串
        }
        p++;  // 移动指针
    }
    if(p != buf) return buf;  // 如果缓冲区不为空，返回已读取的内容
    free(buf);  // 否则释放缓冲区
    return 0;  // 返回空指针，表示没有读取到任何内容
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: xargs [command]\n");  // 没有提供命令时，输出提示信息
        exit(-1);
    }

    char* l;
    argv++;  // argv[0] 是程序名，跳过它，argv[1] 才是实际的命令
    char* nargv[16];  // 存储命令和参数的数组
    char** pna = nargv;  // pna 用于填充 nargv
    char** pa = argv;  // pa 指向传入的命令行参数

    // 将用户输入的命令行参数复制到 nargv 数组中
    while (*pa != 0) {
        *pna = *pa;
        pna++;
        pa++;
    }

    // 循环读取每一行输入
    while ((l = readline()) != 0) {
        char* p = l;
        char* buf = malloc(36);  // 用于存放单个参数的缓冲区
        char* bh = buf;  // bh 指向 buf 的头部
        int nargc = argc - 1;  // 当前参数个数，初始化为传入的命令的参数个数

        // 将读取的一行按照空格分隔成多个参数
        while (*p != 0) {
            if (*p == ' ' && buf != bh) {  // 遇到空格并且当前缓冲区中有字符
                *bh = 0;  // 将缓冲区的内容转换为一个字符串
                nargv[nargc] = buf;  // 将这个参数存储到 nargv 中
                buf = malloc(36);  // 分配新的缓冲区
                bh = buf;  // bh 指向新的缓冲区
                nargc++;  // 参数个数增加
            } else {
                *bh = *p;  // 将当前字符添加到缓冲区中
                bh++;
            }
            p++;
        }

        if (buf != bh) {  // 如果缓冲区中还有未添加的字符
            nargv[nargc] = buf;  // 将缓冲区内容存储为参数
            nargc++;  // 参数个数增加
        }

        nargv[nargc] = 0;  // 参数数组的最后一个元素设为 NULL，表示结束
        free(l);  // 释放读取的输入行

        int pid = fork();  // 创建子进程
        if (pid == 0) {
            exec(nargv[0], nargv);  // 子进程执行用户命令
        } else {
            wait(0);  // 父进程等待子进程完成
        }
    }

    exit(0);
}

