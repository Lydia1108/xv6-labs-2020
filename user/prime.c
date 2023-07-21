#include "kernel/types.h"
#include "user/user.h"

void transmit_non_divisible(int left_pipe[2], int right_pipe[2], int current_prime){
    int data;
    while (read(left_pipe[0], &data, sizeof(int)) == sizeof(int)) {
        if (data % current_prime)
            write(right_pipe[1], &data, sizeof(int));
    }
    close(left_pipe[0]);
    close(right_pipe[1]);
}
void find_primes(int lpipe[2])//寻找素数
{
    close(lpipe[1]);
    int current_prime;
    if (read(lpipe[0], &current_prime, sizeof(int)) != sizeof(int)) {
        exit(0);
    }
    printf("prime %d\n", current_prime);
    // 接收左管道传来的数据
    int p[2];
    pipe(p);
    // 读取左数据。不能整除的数据写入右管道
    transmit_non_divisible(lpipe, p, current_prime);
    
    if (fork() == 0) {
        find_primes(p);//递归
    } 
    else {
        close(p[0]);
        wait(0);
    }
    exit(0);
}
int main(int argc, char const *argv[]){
    int p[2];
    pipe(p);
    for (int i = 2; i <= 35; i++)
        write(p[1], &i, sizeof(int));
    if (fork() == 0) {//父进程
        find_primes(p);
    } 
    else {//子进程
        close(p[1]);
        close(p[0]);
        wait(0);
    }
    exit(0);
}
