#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"
#include "signals.h"

char* readline() {
  char* buffer = (char*)malloc(BUFFER_SIZE*sizeof(char));
  if (buffer == NULL) {
    fprintf(stderr, "myshell: fail to allocate buffer.\n");
    exit(EXIT_FAILURE);
  }
  memset(buffer, 0, BUFFER_SIZE*sizeof(char));
  char c;
  int pos = 0;
  while (1) {
    c = getchar();
    if (c == EOF || c == '\n') {
      if (c == EOF)
        printf("\n");
      buffer[pos] = '\0';
      return buffer;
    } else {
      buffer[pos++] = c;
    }
    if (pos >= BUFFER_SIZE) {
      fprintf(stderr, "myshell: buffer overflow, max 1024 char.\n");
      exit(EXIT_FAILURE);
    }
  }
}
