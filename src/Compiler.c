// © Kay Sievers <kay@versioduo.com>, 2023
// SPDX-License-Identifier: Apache-2.0

// Provide minimal system call stubs for libc newlib.

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

int _close(int) {
  return -1;
}

off_t _lseek(int fd, off_t offset, int whence) {
  return (off_t)-1;
}

int _read(int fd, char *buf, int nbytes) {
  return -1;
}

int _write(int fd, char *buf, int nbytes) {
  return -1;
}

int _fstat(int fd, struct stat *buf) {
  return -1;
}

int _getpid() {
  return 1;
}

int _isatty(int fd) {
  return 0;
}

int _kill(int pid, int sig) {
  return -1;
}
