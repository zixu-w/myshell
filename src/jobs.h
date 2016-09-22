#ifndef _JOBS_H
#define _JOBS_H

#include <unistd.h>
#include "command.h"

typedef struct Process {
  struct Process* next;
  char** argv;
  pid_t pid;
  char completed;
  int status;
} Process;

typedef struct Job {
  struct Job* next;
  char* cmd;
  Process* head;
  pid_t pgid;
  int stdin, stdout;
  int bg;
} Job;

Job* getJob(pid_t);
int jobCompleted(Job*);
void launchProcess(Process*, pid_t, int, int);
int launchJob(Job*);

#endif
