#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

// read a line and put it to buf, return the number of characters read
int readline(char *buf);

int main(int argc, char *argv[]) {
  char buf[256];

  // discard the first argv element, and shift down the rest by one
  int i;
  for (i = 0; i < MAXARG; ++i) {
    argv[i] = argv[i+1];
    if (argv[i] == 0) {
      break;
    }
  }

  while (readline(buf) > 0) {
    // call exec() with an additional argv element
    argv[i] = buf;
    argv[i+1] = 0;

    if (fork() > 0) {
      wait(0);
    } else {
      exec(argv[0], argv);
    }
  }
  exit(0);
}

int readline(char *buf) {
  char *bufp = buf;
  int numRead = 0;

  // read character by character until newline
  while (read(0, bufp, 1) == 1) {
    if (*bufp == '\n') {
      *bufp = 0; // mark null
      break;
    }
    bufp++;
    numRead++;
  }

  return numRead;
}
