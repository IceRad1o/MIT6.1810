#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(const char *path, const char *filename)
{
  int           fd;
  struct stat   st;
  struct dirent de;
  char          buf[512], *p;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    exit(1);
  }

  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  strcpy(buf, path);
  p    = buf + strlen(buf);
  *p++ = '/';
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) { continue; }
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", path);
      continue;
    }
    if (st.type == T_FILE) {
      if (strcmp(de.name, filename) == 0) { printf("%s\n", buf); }
    } else if (st.type == T_DIR) {
      find(buf, filename);
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(2, "Usage: find [dir] [dest]\n");
    exit(1);
  }
  const char *path     = argv[1];
  const char *filename = argv[2];
  find(path, filename);

  exit(0);
}