#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"
#include "signals.h"
#include "parse.h"
#include "jobs.h"

int main(int argc, char const *argv[]) {
  int echo = 0;
  int i;
  for (i = 1; i < argc; ++i)
    if (!strcmp(argv[i], "--echo") || !strcmp(argv[i], "-e"))
      echo = 1;
  setbuf(stdout, NULL);
  char* input = NULL;
  while (1) {
    registerSighandler();
    printf("## myshell $ ");
    input = readline();
    if (input != NULL && strcmp(input, "")) {
      if (echo)
        printf("%s\n", input);
      launchJob(parse(input));
    }
    free(input);
  }
}
