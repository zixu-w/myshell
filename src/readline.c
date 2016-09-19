#include <stdio.h>
#include <stdlib.h>

#include "readline.h"
#include "signals.h"

extern int flag;

char* readline() {
  char* buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
  if (buffer == NULL) {
    fprintf(stderr, "myshell: fail to allocate buffer.\n");
    exit(EXIT_FAILURE);
  }
  char c;
  int pos = 0;
  flag = 1;
  while (flag) {
    c = getchar();
    if (c == EOF || c == '\n') {
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
  return NULL;
}
