#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[]) {
    char buf = 'A'; //用于传送的字节
    int child_to_father[2]; 
    int father_to_child[2]; 
    pipe(child_to_father);
    pipe(father_to_child);
    int pid = fork();//建立子进程
    int exit_status = 0;
    if (pid < 0) {//出错
        printf("error!\n");
        close(child_to_father[0]);
        close(child_to_father[1]);
        close(father_to_child[0]);
        close(father_to_child[1]);
        exit(1);
    } 
    else if (pid == 0) { //pid为0代表子进程
        close(father_to_child[1]);
        close(child_to_father[0]);
        if (read(father_to_child[0], &buf, sizeof(char)) != sizeof(char)) {
            printf("child read() error!\n");
            exit_status = 1; 
        } 
        else {
            printf("%d: received ping\n", getpid());
        }
        if (write(child_to_father[1], &buf, sizeof(char)) != sizeof(char)) {
            printf("child write() error!\n");
            exit_status = 1; 
        }
        close(father_to_child[0]);
        close(child_to_father[1]);
        exit(exit_status);
    } 
    else { //父进程
        close(father_to_child[0]);
        close(child_to_father[1]);
        if (write(father_to_child[1], &buf, sizeof(char)) != sizeof(char)) {
            printf("parent write() error!\n");
            exit_status = 1; 
        }
        if (read(child_to_father[0], &buf, sizeof(char)) != sizeof(char)) {
            printf("parent read() error!\n");
            exit_status = 1; 
        } 
        else {
            printf("%d: received pong\n", getpid());
        }
        close(father_to_child[1]);
        close(child_to_father[0]);
        wait(&exit_status); 
        exit(exit_status);
    }
}
