/*!
   \file readline.c
   \brief Read command lines from input.
   \author WANG Zixu
   \date 20/10/2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"
#include "signals.h"

/*!
   \brief Read a line from input.

   \details
   This function reads a \c '\\n' or \c EOF terminated line from input.

   \return Line buffer read from input.
*/
char* readline() {
  // Allocate read buffer.
  char* buffer = (char*)malloc(BUFFER_SIZE*sizeof(char));
  if (buffer == NULL) {
    fprintf(stderr, "myshell: fail to allocate buffer.\n");
    exit(EXIT_FAILURE);
  }
  memset(buffer, 0, BUFFER_SIZE*sizeof(char));

  char c;
  int pos = 0;

  // Read loop.
  while (1) {
    c = getchar();

    // Return read buffer if reach newline or EOF.
    if (c == EOF || c == '\n') {
      if (c == EOF)
        printf("\n");
      buffer[pos] = '\0';
      return buffer;
    } else {
      // Append character to read buffer.
      buffer[pos++] = c;
    }

    // Raise error if input length exceeds max buffer size.
    if (pos >= BUFFER_SIZE) {
      fprintf(stderr, "myshell: buffer overflow, max 1024 char.\n");
      exit(EXIT_FAILURE);
    }
  }
}
