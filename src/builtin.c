#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "builtin.h"

const char* keywords[] = {
  "exit",
  "cd",
  0
};

builtin_func_ptr builtins[] = {
  &builtin_exit,
  &builtin_cd,
};

builtin_func_ptr map(const char* cmd) {
  int i;
  for (i = 0; keywords[i] != NULL; ++i)
    if (strcmp(cmd, keywords[i]) == 0)
      return builtins[i];
  return NULL;
}

int builtin_cd(char** argv) {
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

int builtin_exit(char** argv) {
  if (argv[1] != NULL) {
    fprintf(stderr, "myshell: \"exit\" with other arguments!!!\n");
    return EXIT_FAILURE;
  }
  else {
    printf("myshell: Terminated.\n");
    exit(EXIT_SUCCESS);
  }
}
