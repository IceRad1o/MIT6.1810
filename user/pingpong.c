#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);

  int pid;
  pid = fork();
  if (pid > 0) {
    // parent
    int  parent_pid = getpid();
    char buf[]      = "b";
    char buf_rev[2];
    if (write(p[1], buf, sizeof(buf)) != 0) { close(p[1]); }
    if (read(p[0], buf_rev, sizeof(buf_rev)) != 0) {
      printf("%d: received pong\n", parent_pid);
      close(p[0]);
    }
    exit(0);
  } else if (pid == 0) {
    int  child_pid = getpid();
    char buf[2];
    if (read(p[0], buf, sizeof(buf)) != 0) {
      printf("%d: received ping\n", child_pid);
      close(p[0]);
    }
    if (write(p[1], buf, sizeof(buf)) != 0) { close(p[1]); }
    exit(0);
  } else {
    fprintf(2, "fork failed\n");
    exit(1);
  }
}