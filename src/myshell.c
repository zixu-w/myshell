#include <stdio.h>
#include <stdlib.h>

#include "readline.h"

int main(int argc, char const *argv[]) {
  setbuf(stdout, NULL);
  char* input = NULL;
  while (1) {
    printf("## myshell $ ");
    input = readline();
    printf("%s\n", input);
  }
  return EXIT_SUCCESS;
}
