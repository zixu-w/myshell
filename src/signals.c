#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "signals.h"

extern volatile sig_atomic_t sigur1Received;
extern volatile sig_atomic_t isTimeX;
extern struct timeval endTime;

void intSighandler(int signum) {}

void cldSighandler(int signum, siginfo_t* sig, void* context) {
  pid_t pid = sig->si_pid;
  pid_t pgid = getpgid(pid);
  if (isTimeX) {
    gettimeofday(&endTime, NULL);
    printf("\n");
    char* filename = (char*)malloc(256*sizeof(char));
    char* procname;
    char* line = (char*)malloc(1024*sizeof(char));
    long double utime = 0L, stime = 0L, rtime;
    if (filename == NULL || line == NULL)
      fprintf(stderr, "myshell: fail to allocate buffer.\n");
    else {
      sprintf(filename, "/proc/%d/stat", pid);
      FILE* file = fopen(filename, "r");
      if (file) {
        fgets(line, 1024, file);
        char* tok = strtok(line, " ");
        size_t i = 1;
        while (tok != NULL) {
          if (i == 2) {
            procname = (char*)malloc((strlen(tok)-1)*sizeof(char));
            size_t j;
            for (j = 0; j < strlen(tok)-2; j++)
              procname[j] = tok[j+1];
            procname[j] = '\0';
          }
          if (i == 14)
            sscanf(tok, "%Lf", &utime);
          if (i == 15)
            sscanf(tok, "%Lf", &stime);
          i++;
          tok = strtok(NULL, " ");
        }
        fclose(file);
        rtime = (
          (long double) (
            ((endTime.tv_sec - startTime.tv_sec) * 1000000L + endTime.tv_usec) - startTime.tv_usec
          )
        ) / 1000000L;
        utime /= sysconf(_SC_CLK_TCK);
        stime /= sysconf(_SC_CLK_TCK);
        printf("PID\tCMD\t\tRTIME\t\tUTIME\t\tSTIME\n");
        printf("%d\t%s\t\t%0.2Lf s\t\t%0.2Lf s\t\t%0.2Lf s\n", pid, procname, rtime, utime, stime);
      } else
        fprintf(stderr, "myshell: fail to open process file.");
      free(filename);
      free(line);
    }
    waitpid(pid, NULL, 0);
  }
  if (pgid == pid) {
    printf("[%d] ", pid);
    char* filename = (char*)malloc(256*sizeof(char));
    if (filename == NULL)
      fprintf(stderr, "myshell: fail to allocate buffer.\n");
    else {
      sprintf(filename, "/proc/%d/comm", pid);
      FILE* file = fopen(filename, "r");
      int c;
      if (file) {
        while ((c = getc(file)) != '\n')
          putchar(c);
        fclose(file);
        printf(" Done\n");
      } else
        fprintf(stderr, "myshell: fail to open process file.");
      waitpid(pid, NULL, 0);
      free(filename);
    }
  }
}

void sigUsr1handler(int signum) {
  sigur1Received = 1;
}

void registerSighandler() {
  struct sigaction sa_int, sa_cld, sa_usr;
  sigaction(SIGINT, NULL, &sa_int);
  sa_int.sa_handler = intSighandler;
  sa_int.sa_flags &= ~SA_RESTART;
  sigaction(SIGINT, &sa_int, NULL);

  sigaction(SIGCHLD, NULL, &sa_cld);
  sa_cld.sa_flags |= SA_SIGINFO;
  sa_cld.sa_flags |= SA_RESTART;
  sa_cld.sa_sigaction = cldSighandler;
  sigaction(SIGCHLD, &sa_cld, NULL);

  sigaction(SIGUSR1, NULL, &sa_usr);
  sa_usr.sa_handler = sigUsr1handler;
  sigaction(SIGUSR1, &sa_usr, NULL);
}
