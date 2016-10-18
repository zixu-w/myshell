#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <sys/wait.h>
#include <signal.h>

#include "jobs.h"
#include "builtin.h"

extern char* program_invocation_name;
extern char* program_invocation_short_name;
extern volatile sig_atomic_t sigur1Received;

pid_t fpgid;

void launchProcess(Process* p, pid_t pgid, int in, int out) {
  if (in != STDIN_FILENO) {
    dup2(in, STDIN_FILENO);
    close(in);
  }
  if (out != STDOUT_FILENO) {
    dup2(out, STDOUT_FILENO);
    close(out);
  }
  setpgid(0, pgid);
  while (!sigur1Received)
    ;
  sigur1Received = 0;
  if (execvp(p->argv[0], p->argv) == -1)
    error(EXIT_FAILURE, errno, "'%s'", p->argv[0]);
  exit(EXIT_FAILURE);
}

int launchJob(Job* j) {
  if (j == NULL)
    return -1;
  fpgid = getpgid(0);
  if (j->bg)
    j->pgid = 0;
  else {
    j->pgid = fpgid;
    signal(SIGINT, SIG_IGN);
  }
  sigur1Received = 0;
  program_invocation_name = program_invocation_short_name;
  Process* p = j->head;
  pid_t pid;
  int status = EXIT_SUCCESS;
  if (p->next != NULL) {
    int mypipe[2], in, out;
    in = j->stdin;
    for (; p; p = p->next) {
      if (p->next) {
        if (pipe(mypipe) < 0)
          error(EXIT_FAILURE, errno, "pipe");
        out = mypipe[1];
      } else
        out = j->stdout;
      builtin_func_ptr builtin = map(p->argv[0]);
      if (builtin != NULL) {
        pid = fork();
        if (pid == 0) {
          exit(builtin(p->argv));
        }
        else if (pid < 0)
          error(EXIT_FAILURE, errno, "fork");
        else {
          p->pid = pid;
          kill(pid, SIGUSR1);
        }
      } else {
        pid = fork();
        if (pid == 0)
          launchProcess(p, j->pgid, in, out);
        else if (pid < 0)
          error(EXIT_FAILURE, errno, "fork");
        else {
          p->pid = pid;
          kill(pid, SIGUSR1);
        }
      }
      if (in != j->stdin)
        close(in);
      if (out != j->stdout)
        close(out);
      in = mypipe[0];
    }
    for (p = j->head; p; p = p->next)
      if (p->pid >= 0)
        waitpid(p->pid, &status, j->bg);
  } else {
    builtin_func_ptr builtin = map(p->argv[0]);
    if (builtin != NULL)
      return builtin(p->argv);
    pid = fork();
    if (pid == 0)
      launchProcess(p, j->pgid, j->stdin, j->stdout);
    else if (pid < 0)
      error(EXIT_FAILURE, errno, "fork");
    else {
      p->pid = pid;
      kill(pid, SIGUSR1);
      waitpid(pid, &status, j->bg);
      if (!j->bg)
        return WEXITSTATUS(status);
    }
  }
  return WEXITSTATUS(status);
}
