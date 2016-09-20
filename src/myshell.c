#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"
#include "signals.h"
#include "parse.h"
#include "jobs.h"

int main(int argc, char const *argv[]) {
  setbuf(stdout, NULL);
  char* input = NULL;
  registerSighandler();
  while (1) {
    printf("## myshell $ ");
    input = readline();
    if (input != NULL)
      launch(parse(split(input)));
    free(input);
  }
  return EXIT_SUCCESS;
}
