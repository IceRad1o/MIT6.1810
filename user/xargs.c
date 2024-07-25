#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
readarg(int arg_num, char *args[])
{
  char buf[512];
  int  offset = 0;
  while (read(0, buf + offset, 1) != 0) {
    if (offset > 512) {
      fprintf(2, "xargs: args too long\n");
      exit(1);
    }

    if (buf[offset] == '\n') { break; }
    offset++;
  }
  if (offset == 0) { return 0; }
  // replace '\n' with '\0'
  buf[offset]   = 0;
  args[arg_num] = malloc(offset + 1);
  strcpy(args[arg_num], buf);
  return offset;
}

int
main(int argc, char *argv[])
{
  if (argc < 2) { fprintf(2, "Usage: xargs [command] [args]\n"); }

  char *args[MAXARG];

  char *command = argv[1];
  for (int i = 1; i < argc; i++) { args[i - 1] = argv[i]; }

  int arg_num = argc - 1;
  while (readarg(arg_num, args)) {
    if (fork() == 0) {
      exec(command, args);
      exit(0);
    } else {
      wait(0);
    }
  }
  exit(0);
}