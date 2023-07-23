#include "kernel/types.h"
#include "user/user.h"

void transmit_data(int lpipe[2], int rpipe[2], int first){
    int data;
    while (read(lpipe[0], &data, sizeof(int)) == sizeof(int)) {
        if (data % first)
            write(rpipe[1], &data, sizeof(int));
    }
    close(lpipe[0]);
    close(rpipe[1]);
}

void primes(int lpipe[2]){
    close(lpipe[1]);
    int first;
    if (read(lpipe[0], &first, sizeof(int)) != sizeof(int)) {
        exit(0);
    }
    printf("prime %d\n", first);
    int p[2];
    pipe(p);
    transmit_data(lpipe, p, first);
    // 递归
    if (fork() == 0) {
        primes(p);
    } else {
        close(p[0]);
        wait(0);
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    int p[2];
    pipe(p);
    for (int i = 2; i <= 35; i++)
        write(p[1], &i, sizeof(int));
    if (fork() == 0) {//父进程
        primes(p);
    } 
    else {//子进程
        close(p[1]);
        close(p[0]);
        wait(0);
    }
    exit(0);
}