#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

//参考ls.c文件
void find(char *path, const char *filename)//2个参数：文件目录，待寻找的文件名
{
  char buf[512], *p;//缓冲区，指针
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {//无法打开对应文件
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot fstat %s\n", path);
    close(fd);
    return;
  }

  //打印出提示：find的第一个参数错误，应为目录
  if (st.type != T_DIR) {
    fprintf(2, "usage: find <DIRECTORY> <filename>\n");
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {//超出长度
    fprintf(2, "find: path too long\n");
    return;
  }
  strcpy(buf, path);//copy字符串
  p = buf + strlen(buf);
  *p++ = '/'; //p指向最后一个'/'后方
  while (read(fd, &de, sizeof de) == sizeof de) {
    if (de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ); //添加路径名称
    p[DIRSIZ] = 0;               //字符串结束标志
    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }
    //不能在“.”和“..”目录中递归
    if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {//p不指向"."或".."
      find(buf, filename);//深度优先搜索
    } 
    else if (strcmp(filename, p) == 0)
      printf("%s\n", buf);
  }

  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {//检查参数情况
    fprintf(2, "usage: find <directory> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
