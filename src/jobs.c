#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <sys/wait.h>

#include "jobs.h"

extern char* program_invocation_name;
extern char* program_invocation_short_name;

int launch(Command_t** commands) {
  Command_t command = *commands[0];
  pid_t pid;
  int status;
  program_invocation_name = program_invocation_short_name;
  pid = fork();
  if (pid == 0) {
    if (execvp(command.cmd, command.argv) == -1) {
      error(EXIT_FAILURE, errno, "'%s'", command.cmd);
    }
  } else if (pid < 0) {
    perror("myshell");
    exit(EXIT_FAILURE);
  }
  int options = 0;
  if (command.bg_flag)
    options |= WNOHANG;
  int i;
  for (i = 0; commands[i] != NULL; ++i) {
    free(commands[i]->argv);
  }
  free(commands);
  waitpid(pid, &status, options);
  return WIFEXITED(status);
}
