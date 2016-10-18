#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "signals.h"

void intSighandler(int signum) {}

void cldSighandler(int signum, siginfo_t* sig, void* context) {
  pid_t pid = sig->si_pid;
  pid_t pgid = getpgid(pid);
  if (pgid == pid) {
    printf("[%d] ", pid);
    char* filename = (char*)malloc(256*sizeof(char));
    sprintf(filename, "/proc/%d/comm", pid);
    FILE* file = fopen(filename, "r");
    int c;
    if (file) {
      while ((c = getc(file)) != '\n')
        putchar(c);
      fclose(file);
    }
    printf(" Done\n");
    waitpid(pid, NULL, 0);
  }
}

void registerSighandler() {
  struct sigaction sa_int, sa_cld;
  sigaction(SIGINT, NULL, &sa_int);
  sa_int.sa_handler = intSighandler;
  sa_int.sa_flags &= ~SA_RESTART;
  sigaction(SIGINT, &sa_int, NULL);

  sigaction(SIGCHLD, NULL, &sa_cld);
  sa_cld.sa_flags |= SA_SIGINFO;
  sa_cld.sa_flags |= SA_RESTART;
  sa_cld.sa_sigaction = cldSighandler;
  sigaction(SIGCHLD, &sa_cld, NULL);
}
