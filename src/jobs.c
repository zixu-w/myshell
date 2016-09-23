#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <sys/wait.h>

#include "jobs.h"
#include "builtin.h"

extern char* program_invocation_name;
extern char* program_invocation_short_name;

Job* head = NULL;

Job* getJob(pid_t pgid) {
  Job* j;
  for (j = head; j; j = j->next)
    if (j->pgid == pgid)
      return j;
  return NULL;
}

int jobCompleted(Job* j) {
  Process* p;
  for (p = j->head; p; p = p->next)
    if (!p->completed)
      return 0;
  return 1;
}

void launchProcess(Process* p, pid_t pgid, int in, int out) {
  if (in != STDIN_FILENO) {
    dup2(in, STDIN_FILENO);
    close(in);
  }
  if (out != STDOUT_FILENO) {
    dup2(out, STDOUT_FILENO);
    close(out);
  }
  if (execvp(p->argv[0], p->argv) == -1)
    error(EXIT_FAILURE, errno, "'%s'", p->argv[0]);
  exit(EXIT_FAILURE);
}

int launchJob(Job* j) {
  if (j == NULL)
    return -1;
  program_invocation_name = program_invocation_short_name;
  Process* p;
  pid_t pid;
  int mypipe[2] = {0}, in, out, status;
  in = j->stdin;
  for (p = j->head; p; p = p->next) {
    if (p->next) {
      if (pipe(mypipe) < 0)
        error(EXIT_FAILURE, errno, "pipe");
      out = mypipe[1];
    } else
      out = j->stdout;
    builtin_func_ptr builtin = map(p->argv[0]);
    if (builtin != NULL)
      return builtin(p->argv);
    pid = fork();
    if (pid == 0)
      launchProcess(p, j->pgid, in, out);
    else if (pid < 0)
      error(EXIT_FAILURE, errno, "fork");
    else {
      p->pid = pid;
    }
    if (in != j->stdin)
      close(in);
    if (out != j->stdout)
      close(out);
    in = mypipe[0];
  }
  p = j->head;
  while (p != NULL) {
    waitpid(p->pid, &status, 0);
    p = p->next;
  }
  return 0;
}
