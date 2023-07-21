#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[]) {
    char buf = 'A'; //用于传送的字节
    // 0代表读，1代表写
    int child_to_father[2]; //子进程->父进程
    int father_to_child[2]; //父进程->子进程
    pipe(child_to_father);
    pipe(father_to_child);
    // 使用fork创建子进程
    int pid = fork();
    int exit_status = 0;
    // 错误情况
    if (pid < 0) {
        printf("error!\n");
        close(child_to_father[0]);
        close(child_to_father[1]);
        close(father_to_child[0]);
        close(father_to_child[1]);
        exit(1);
    } else if (pid == 0) { //子进程
        close(father_to_child[1]);
        close(child_to_father[0]);
        if (read(father_to_child[0], &buf, sizeof(char)) != sizeof(char)) {
            printf("child read() error!\n");
            exit_status = 1; //标记出错
        } else {
            printf("%d: received ping\n", getpid());
        }
        if (write(child_to_father[1], &buf, sizeof(char)) != sizeof(char)) {
            printf("child write() error!\n");
            exit_status = 1; //标记出错
        }
        close(father_to_child[0]);
        close(child_to_father[1]);
        exit(exit_status);
    } else { //父进程
        close(father_to_child[0]);
        close(child_to_father[1]);
        if (write(father_to_child[1], &buf, sizeof(char)) != sizeof(char)) {
            printf("parent write() error!\n");
            exit_status = 1; //标记写出错
        }
        if (read(child_to_father[0], &buf, sizeof(char)) != sizeof(char)) {
            printf("parent read() error!\n");
            exit_status = 1; //标记出错
        } else {
            printf("%d: received pong\n", getpid());
        }
        close(father_to_child[1]);
        close(child_to_father[0]);
        wait(&exit_status); //等待子进程结束
        exit(exit_status);
    }
}
