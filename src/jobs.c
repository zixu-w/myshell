#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "jobs.h"

int launch(Command_t** commands) {
  Command_t command = *commands[0];
  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0) {
    if (execvp(command.cmd, command.argv) == -1) {
      perror("myshell");
      exit(EXIT_FAILURE);
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
