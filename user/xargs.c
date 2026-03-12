#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int readline(char *buf, int max) {
  int n = 0;
  while (n + 1 < max) {
    int r = read(0, buf + n, 1);
    if (r < 1) break;
    if (buf[n] == '\n') { 
      buf[n] = 0;
      return n;
    }
    n++;
  }
  buf[n] = 0;
  return n;
}

int
main(int argc, char *argv[])
{
  char buf[512];
  char *args[MAXARG];
  int i;

  for (i = 1; i < argc; i++) {
    args[i - 1] = argv[i];
  }

  while (readline(buf, sizeof(buf)) > 0) {

    args[argc - 1] = buf;
    args[argc] = 0;

    if (fork() == 0) {
      exec(args[0], args);
      fprintf(2, "xargs: exec %s failed\n", args[0]);
      exit(1);
    }
    wait(0);
  }

  exit(0);
}
