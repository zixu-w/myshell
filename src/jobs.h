#ifndef _JOBS_H
#define _JOBS_H

#include <unistd.h>

typedef struct Process {
  struct Process* next;
  char** argv;
  pid_t pid;
} Process;

typedef struct Job {
  char* cmd;
  Process* head;
  pid_t pgid;
  int stdin, stdout;
  int bg;
  int isTimeX;
} Job;

int launchJob(Job*);

#endif
