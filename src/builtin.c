#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "builtin.h"
#include "signals.h"

extern volatile sig_atomic_t isTimeX;

const char* keywords[] = {
  "exit",
  "cd",
  "timeX",
  0
};

builtin_func_ptr builtins[] = {
  &builtin_exit,
  &builtin_cd,
  &builtin_timeX,
};

builtin_func_ptr map(const char* cmd) {
  int i;
  for (i = 0; keywords[i] != NULL; ++i)
    if (strcmp(cmd, keywords[i]) == 0)
      return builtins[i];
  return NULL;
}

int builtin_cd(char** argv, Job* j) {
  int i, status;
  char dst[ARGV_SIZE] = {0};
  for (i = 1; argv[i] != NULL; ++i) {
    strcat(dst, argv[i]);
    strcat(dst, "/");
  }
  if (strcmp(dst, "") == 0)
    status = chdir(getenv("HOME"));
  else
    status = chdir(dst);
  if (status != 0) {
    perror("cd");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int builtin_exit(char** argv, Job* j) {
  if (argv[1] != NULL || j->bg) {
    fprintf(stderr, "myshell: \"exit\" with other arguments!!!\n");
    return EXIT_FAILURE;
  }
  else {
    printf("myshell: Terminated.\n");
    exit(EXIT_SUCCESS);
  }
}

int builtin_timeX(char** argv, Job* j) {
  if (j->bg) {
    fprintf(stderr, "myshell: \"timeX\" cannot be run in background mode\n");
    return EXIT_FAILURE;
  }
  if (argv[1] == NULL) {
    if (j->head->next != NULL)
      fprintf(stderr, "myshell: Cannot timeX an incomplete '|' sequence\n");
    else
      fprintf(stderr, "myshell: \"timeX\" cannot be a standalone command\n");
    return EXIT_FAILURE;
  }
  size_t i;
  for (i = 1; argv[i]; i++);
  char** cmdArgv = (char**)malloc(i*sizeof(char*));
  for (i = 0; argv[i+1]; i++)
    cmdArgv[i] = argv[i+1];
  free(argv);
  j->head->argv = cmdArgv;
  isTimeX = 1;
  return launchJob(j);
}
