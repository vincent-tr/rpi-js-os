// https://sourceware.org/newlib/libc.html#Syscalls

extern "C" {

#include <sys/stat.h>
#include <errno.h>
#undef errno
extern int errno;
}


#include "syscalls_def.hh"

namespace support {
  void (*exit_impl)(int status);
}

extern "C" {

void _exit(int status) {
  support::exit_impl(status);
}

int close(int file) {
  (void)file;
  return -1;
}

char *__env[1] = { 0 };
char **environ = __env;

int execve(char *name, char **argv, char **env) {
  (void)name;
  (void)argv;
  (void)env;
  errno = ENOMEM;
  return -1;
}

int fork(void) {
  errno = EAGAIN;
  return -1;
}

int fstat(int file, struct stat *st) {
  (void)file;
  (void)st;
  st->st_mode = S_IFCHR;
  return 0;
}

int getpid(void) {
  return 1;
}

int isatty(int file) {
  (void)file;
  return 1;
}

int kill(int pid, int sig) {
  (void)pid;
  (void)sig;
  errno = EINVAL;
  return -1;
}

int link(char *old, char *_new) {
  (void)old;
  (void)_new;
  errno = EMLINK;
  return -1;
}

int lseek(int file, int ptr, int dir) {
  (void)file;
  (void)ptr;
  (void)dir;
  return 0;
}

int open(const char *name, int flags, int mode) {
  (void)name;
  (void)flags;
  (void)mode;
  return -1;
}

int read(int file, char *ptr, int len) {
  (void)file;
  (void)ptr;
  (void)len;
  return 0;
}

// caddr_t sbrk(int incr)
// Increase program data space. As malloc and related functions depend on this, it is useful to have a working implementation. The following suffices for a standalone system; it exploits the symbol _end automatically defined by the GNU linker.

int stat(const char *file, struct stat *st) {
  (void)file;
  (void)st;
  st->st_mode = S_IFCHR;
  return 0;
}

int times(struct tms *buf) {
  (void)buf;
  return -1;
}

int unlink(char *name) {
  (void)name;
  errno = ENOENT;
  return -1;
}

int wait(int *status) {
  (void)status;
  errno = ECHILD;
  return -1;
}

int write(int file, char *ptr, int len) {
  (void)file;
  (void)ptr;
  (void)len;
  return 0;
}

/*
Write to a file. libc subroutines will use this system routine for output to all files, including stdout—so if you need to generate any output, for example to a serial port for debugging, you should make your minimal write capable of doing this. The following minimal implementation is an incomplete example; it relies on a outbyte subroutine (not shown; typically, you must write this in assembler from examples provided by your hardware manufacturer) to actually perform the output.

int write(int file, char *ptr, int len) {
  int todo;

  for (todo = 0; todo < len; todo++) {
    outbyte (*ptr++);
  }
  return len;
}
*/

// export symbols ?!?
int (*_close)(int file) = close;
int (*_execve)(char *name, char **argv, char **env) = execve;
int (*_fork)(void) = fork;
int (*_fstat)(int file, struct stat *st) = fstat;
int (*_getpid)(void) = getpid;
int (*_isatty)(int file) = isatty;
int (*_kill)(int pid, int sig) = kill;
int (*_link)(char *old, char *_new) = link;
int (*_lseek)(int file, int ptr, int dir) = lseek;
int (*_open)(const char *name, int flags, int mode) = open;
int (*_read)(int file, char *ptr, int len) = read;
int (*_stat)(const char *file, struct stat *st) = stat;
int (*_times)(struct tms *buf) = times;
int (*_unlink)(char *name) = unlink;
int (*_wait)(int *status) = wait;
int (*_write)(int file, char *ptr, int len) = write;

}
