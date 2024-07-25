#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
primepipeline(int *p)
{
  close(p[1]);
  int prime;
  if (read(p[0], &prime, 4) != 0) {
    printf("prime %d\n", prime);
  } else {
    fprintf(2, "read prime failed");
    exit(1);
  }

  int newp[2];
  pipe(newp);
  if (fork() > 0) {
    close(newp[0]);
    int n;
    while (read(p[0], &n, 4)) {
      if (n % prime != 0) { write(newp[1], &n, 4); }
    }
    close(p[0]);
    close(newp[1]);
    wait(0);
  } else {
    primepipeline(newp);
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  if (fork() > 0) {
    close(p[0]);
    for (int i = 2; i <= 35; i++) { write(p[1], &i, 4); }
    close(p[1]);
    wait(0);
  } else {
    primepipeline(p);
  }
  exit(0);
}