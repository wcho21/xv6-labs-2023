#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int p[2];
  pipe(p);

  char buf[32];

  if (fork() == 0) { // child process
    // get ping from the parent
    read(p[0], buf, sizeof(buf));

    printf("%d: received %s\n", getpid(), buf);

    // send pong to the parent
    write(p[1], "pong", 4);

    // release
    close(p[0]);
    close(p[1]);
  } else { // parent process
    // send ping to the child
    write(p[1], "ping", 4);

    // get pong from the child
    read(p[0], buf, sizeof(buf));

    printf("%d: received %s\n", getpid(), buf);

    // release
    close(p[0]);
    close(p[1]);

    wait(0);
  }

  exit(0);
}
