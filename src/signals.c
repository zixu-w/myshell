/*!
   \file signals.c
   \brief Register and handle signals.
   \author WANG Zixu
   \date 20/10/2016
*/

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "signals.h"

/*!
  \fn void intSighandler(int signum)
  \brief Dumb \c SIGINT handler.

  \param signum Signal number.
*/
void intSighandler(int signum) {}

/*!
  \fn void cldSighandler(int signum, siginfo_t* sig, void* context)
  \brief Handle \c SIGCHLD from terminated child processes.

  \details
  This signal handler catches \c SIGCHLD and handles cases of \c timeX processes
  and background processes.
  For \c timeX processes, it calculates the real running time of the processes,
  reads \c /proc/<pid>/stat file to get information, prints the information,
  and then releases the processes.
  For background processes, it reads command name from \c /proc/<pid>/comm file,
  informs done, and releases the processes.

  \param signum Signal number.
  \param sig Signal information.
  \param context Context pointer.

  \bug
  The handler is not thread-safe. The processing time may be too long that there
  may be another \c SIGCHLD signal arriving when the handler is handling a
  previous \c SIGCHLD signal.
  This may lead to defunct processes and incomplete output.
*/
void cldSighandler(int signum, siginfo_t* sig, void* context) {
  // Get sender process id.
  pid_t pid = sig->si_pid;

  // Get sender group process id to see if it's a background process.
  pid_t pgid = getpgid(pid);

  // If is timeX process.
  if (isTimeX) {
    // Record end time stamp.
    gettimeofday(&endTime, NULL);

    // Print a leading newline.
    printf("\n");

    char* filename = (char*)malloc(256*sizeof(char));
    char* procname;
    char* line = (char*)malloc(1024*sizeof(char));
    long double utime = 0L, stime = 0L, rtime;
    if (filename == NULL || line == NULL)
      fprintf(stderr, "myshell: fail to allocate buffer.\n");
    else {
      // Open and read /proc/<pid>/stat file.
      sprintf(filename, "/proc/%d/stat", pid);
      FILE* file = fopen(filename, "r");
      if (file) {
        line = fgets(line, 1024, file);
        char* tok = strtok(line, " ");
        size_t i = 1;
        while (tok != NULL) {

          // Second column, read process name.
          if (i == 2) {
            procname = (char*)malloc((strlen(tok)-1)*sizeof(char));
            size_t j;
            for (j = 0; j < strlen(tok)-2; j++)
              procname[j] = tok[j+1];
            procname[j] = '\0';
          }

          // 14th column, read utime.
          if (i == 14)
            sscanf(tok, "%Lf", &utime);

          // 15th column, read stime and finish reading.
          if (i == 15) {
            sscanf(tok, "%Lf", &stime);
            break;
          }

          // Next column.
          i++;
          tok = strtok(NULL, " ");
        }
        fclose(file);

        // Calculate real running time by subtracting start time stamp
        // from end time stamp.
        rtime = (
          (long double) (
            ((endTime.tv_sec - startTime.tv_sec) * 1000000L + endTime.tv_usec) - startTime.tv_usec
          )
        ) / 1000000L;

        // Calculate utime and stime in seconds.
        utime /= sysconf(_SC_CLK_TCK);
        stime /= sysconf(_SC_CLK_TCK);

        // Format output.
        printf("PID\tCMD\t\tRTIME\t\tUTIME\t\tSTIME\n");
        printf("%d\t%s\t\t%0.2Lf s\t\t%0.2Lf s\t\t%0.2Lf s\n", pid, procname, rtime, utime, stime);
      } else
        fprintf(stderr, "myshell: fail to open process file.");

      // Release memory.
      free(filename);
      free(line);
    }

    // Release process.
    waitpid(pid, NULL, 0);
  }

  // If is background process.
  if (pgid == pid) {
    // Format output process id.
    printf("[%d] ", pid);

    char* filename = (char*)malloc(256*sizeof(char));
    if (filename == NULL)
      fprintf(stderr, "myshell: fail to allocate buffer.\n");
    else {
      // Open and read /proc/<pid>/comm file.
      sprintf(filename, "/proc/%d/comm", pid);
      FILE* file = fopen(filename, "r");
      int c;
      if (file) {
        // Read and print process name.
        while ((c = getc(file)) != '\n')
          putchar(c);
        fclose(file);

        // Inform done.
        printf(" Done\n");
      } else
        fprintf(stderr, "myshell: fail to open process file.");

      // Release process and memory.
      waitpid(pid, NULL, 0);
      free(filename);
    }
  }
}

/*!
  \fn void sigUsr1handler(int signum)
  \brief \c SIGUSR1 handler.

  \details
  This signal handler catches \c SIGUSR1 and set flag sigur1Received
  upon receiving.

  \param signum Signal number.
*/
void sigUsr1handler(int signum) {
  sigur1Received = 1;
}

/*!
  \brief Register signal handlers.

  \details
  This function registers required signal handlers.
*/
void registerSighandler() {
  // Declare sigaction structs.
  struct sigaction sa_int, sa_cld, sa_usr;

  // Retrieve default sigaction for SIGINT.
  sigaction(SIGINT, NULL, &sa_int);
  // Set handler.
  sa_int.sa_handler = intSighandler;
  // Unset SA_RESTART option.
  sa_int.sa_flags &= ~SA_RESTART;
  // Register sigaction.
  sigaction(SIGINT, &sa_int, NULL);

  // Retrieve default sigaction for SIGCHLD.
  sigaction(SIGCHLD, NULL, &sa_cld);
  // Set SA_SIGINFO option to allow information retrieving.
  sa_cld.sa_flags |= SA_SIGINFO;
  // Set SA_RESTART option.
  sa_cld.sa_flags |= SA_RESTART;
  // Set SA_NOCLDSTOP option.
  sa_cld.sa_flags |= SA_NOCLDSTOP;
  // Set handler.
  sa_cld.sa_sigaction = cldSighandler;
  // Register sigaction.
  sigaction(SIGCHLD, &sa_cld, NULL);

  // Retrieve default sigaction for SIGUSR1.
  sigaction(SIGUSR1, NULL, &sa_usr);
  // Set handler.
  sa_usr.sa_handler = sigUsr1handler;
  // Register sigaction.
  sigaction(SIGUSR1, &sa_usr, NULL);
}
