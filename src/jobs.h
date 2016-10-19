#ifndef _JOBS_H
#define _JOBS_H

#include <unistd.h>

typedef struct Process {
  struct Process* next;
  char** argv;
  pid_t pid;
} Process;

typedef struct Job {
  Process* head;
  pid_t pgid;
  int stdin, stdout;
  int bg;
} Job;

int launchJob(Job*);
void cleanJob(Job*);
void cleanProcess(Process*);

#endif
