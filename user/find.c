#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *dir, char *name);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "usage: find <directory> <name>\n");
    exit(1);
  }

  char *dir = argv[1];
  char *name = argv[2];

  find(dir, name);

  exit(0);
}

void find(char *dir, char *name) {
  char buf[512];

  // handle exceptions
  int fd;
  if ((fd = open(dir, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", dir);
    return;
  }
  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", dir);
    close(fd);
    return;
  }
  if (st.type != T_DIR) {
    fprintf(2, "find: not a directory %s\n", dir);
    return;
  }
  if (strlen(dir) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    fprintf(2, "find: path too long\n");
    return;
  }

  // list found files and recurse over sub-directories
  struct dirent de;
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0) { // don't know what it is, but seems necessary to get rid of blanks
      continue;
    }

    // skip . and ..
    if (strcmp(de.name, ".") == 0) {
      continue;
    }
    if (strcmp(de.name, "..") == 0) {
      continue;
    }

    // get the full path to buf
    strcpy(buf, dir);
    char *p = buf + strlen(buf);
    *p++ = '/';
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    // list if found
    if (strcmp(de.name, name) == 0) {
      printf("%s\n", buf);
    }

    // recurse if a directory
    if (stat(buf, &st) < 0) {
      fprintf(2, "find: not a directory %s\n", buf);
      continue;
    }
    if (st.type == T_DIR) {
      find(buf, name);
    }
  }

  close(fd);
}
