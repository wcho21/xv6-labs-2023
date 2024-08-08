#include "kernel/types.h"
#include "user/user.h"

void sieve(int readPipe);

int main(int argc, char *argv[]) {
  int p[2];
  pipe(p);

  if (fork() > 0) { // parent process
    close(p[0]);

    // send integer with pipe (see [1])
    for (int i = 2; i <= 35; ++i) {
      write(p[1], &i, sizeof(i));
    }

    close(p[1]);

    wait(0);
  } else {
    close(p[1]);

    sieve(p[0]);

    close(p[0]);
  }

  exit(0);
}

void sieve(int readPipe) {
  // if a number is sent, print it as the first prime number; otherwise, exit
  int n;
  if (read(readPipe, &n, sizeof(n)) > 0) {
    printf("prime %d\n", n);
  } else {
    return;
  }

  int p[2];
  pipe(p);

  if (fork() > 0) { // parent process
    close(p[0]);

    // send numbers if not divided by the first number n
    int m;
    while (read(readPipe, &m, sizeof(m)) > 0) {
      if (m % n > 0) {
        write(p[1], &m, sizeof(m));
      }
    }

    close(p[1]);

    wait(0);
  } else { // child process
    close(p[1]);

    // recursively sieve numbers
    sieve(p[0]);

    close(p[0]);
  }

  exit(0);
}

/*
 * reference:
 * [1] How to send integer with pipe between two process
 *     https://stackoverflow.com/questions/5237041/how-to-send-integer-with-pipe-between-two-processes
 */
