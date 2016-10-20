/*!
   \file myshell.c
   \brief Entry point of myshell.
   \details This file contains the main logic loop of myshell.
   \author WANG Zixu
   \date 20/10/2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"
#include "signals.h"
#include "parse.h"
#include "jobs.h"

/*!
   \fn int main(int argc, char const *argv[])
   \brief Main function. Entry point of myshell.

   \details
   Main logic loop of myshell.
   Prompt for input, parse the input command line, and launch the job.

   \param argc Argument count.
   \param argv Argument vector.

   \return The function does not return from the main function.
*/
int main(int argc, char const *argv[]) {
  // Flag for echo when running test script.
  int echo = 0;

  // Read arguments.
  int i;
  for (i = 1; i < argc; ++i)
    // If found arguments ask for echo, set echo flag.
    if (!strcmp(argv[i], "--echo") || !strcmp(argv[i], "-e"))
      echo = 1;

  // Flush standard output immediately.
  setbuf(stdout, NULL);

  // Input buffer.
  char* input = NULL;

  // Main REPL loop.
  while (1) {
    // Re-register signal handler for safety.
    registerSighandler();

    // Prompt.
    printf("## myshell $ ");

    // Read a command line.
    input = readline();

    // If the command line is not empty.
    if (input != NULL && strcmp(input, "")) {
      // Echo back the input if echo flag is set.
      if (echo)
        printf("%s\n", input);

      // Parse and launch the job.
      launchJob(parse(input));
    }

    // Release the input buffer for the next iteration.
    free(input);
  }
}
