/*!
   \file jobs.c
   \brief Job and \link Process process \endlink launching, management, and clean up.
   \author WANG Zixu
   \date 20/10/2016
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#include "jobs.h"
#include "builtin.h"
#include "signals.h"

/*!
   \var extern char* program_invocation_name
   \brief External variable to set the error prompt.
*/
extern char* program_invocation_name;

/*!
   \var extern char* program_invocation_short_name
   \brief External variable to set the error prompt.
*/
extern char* program_invocation_short_name;

/*!
   \var pid_t fpgid
   \brief Store foreground process group id.
*/
pid_t fpgid;

/*!
   \brief Launch a \link Process process \endlink.

   \details
   This function is called in child processes after \c fork().
   It redirects input/output of process \p p for piping to \p in and \p out
   if they are not standard input/output.
   It then sets the process id of \p p to \p pgid for background/foreground
   management. After receiving the signal SIGUSR1, it calls \c execvp() to
   invoke the command of the process.

   \param p Pointer to the process to be launched.
   \param pgid Process group id to be set to the process.
   \param in Input file descriptor.
   \param out Output file descriptor.
*/
void launchProcess(Process* p, pid_t pgid, int in, int out) {
  // If input is redirected, alter file descriptor.
  if (in != STDIN_FILENO) {
    dup2(in, STDIN_FILENO);
    close(in);
  }

  // If output is redirected, alter file descriptor.
  if (out != STDOUT_FILENO) {
    dup2(out, STDOUT_FILENO);
    close(out);
  }

  // Set the group process id.
  // Group process id is set to pid of p if pgid equals to 0.
  // This way we can deal with background processes seperately.
  setpgid(0, pgid);

  // Block untill SIGUSR1 has been received.
  while (!sigur1Received)
    ;
  sigur1Received = 0;

  // Invoke the command. Prompt error message and exit if there's an error.
  if (execvp(p->argv[0], p->argv) == -1)
    error(EXIT_FAILURE, errno, "'%s'", p->argv[0]);
  exit(EXIT_FAILURE);
}

/*!
   \brief Launch a \link Job job \endlink.

   \details
   This function takes a job and launches it.
   It handles background/foreground setting, pipe setting, builtin function
   looking up before invoking the process(es).
   It then calls \link launchProcess launchProcess() \endlink to launch the
   process(es) and waits for foreground process(es) to terminate.
   Then it performs cleaning operations.

   \param j Pointer to the job to be launched.

   \return Exit status.

   \sa launchProcess
   \sa cleanJob
*/
int launchJob(Job* j) {
  if (j == NULL)
    return -1;

  // Get group process id for foreground process group.
  fpgid = getpgid(0);

  // Let group process id to be 0 if the job is in background mode.
  if (j->bg)
    j->pgid = 0;

  // Else set group process id to foreground group process id.
  // Also ignore SIGINT for myshell if in foreground mode.
  else {
    j->pgid = fpgid;
    signal(SIGINT, SIG_IGN);
  }

  // Initialize flag for SIGUSR1.
  sigur1Received = 0;

  // Set the error prompt file name to short name 'myshell'.
  program_invocation_name = program_invocation_short_name;

  Process* p = j->head;
  pid_t pid;
  int status = EXIT_SUCCESS;

  // If the job involves pipe.
  if (p->next != NULL && strcmp(p->argv[0], "timeX")) {
    int mypipe[2], in, out;
    in = j->stdin;

    // Set up pipe and launch processes in the pipe.
    for (; p; p = p->next) {
      // Redirect output if not at end of pipe.
      if (p->next) {
        if (pipe(mypipe) < 0)
          error(EXIT_FAILURE, errno, "pipe");
        out = mypipe[1];
      } else
        // At end of pipe, set output to standard output.
        out = j->stdout;

      // Check if the command is a builtin keyword.
      builtin_func_ptr builtin = map(p->argv[0]);

      // If is builtin command.
      if (builtin != NULL) {
        pid = fork();
        if (pid == 0) {
          // In child process, invoke the command.
          exit(builtin(p->argv, j));
        }
        else if (pid < 0)
          // Error when forking.
          error(EXIT_FAILURE, errno, "fork");
        else {
          // In parent process, set process id
          // and send SIGUSR1 to let child process run.
          p->pid = pid;
          kill(pid, SIGUSR1);
        }
      } else {
        // If is NOT builtin command.
        pid = fork();
        if (pid == 0)
          // In child process, call launchProcess.
          launchProcess(p, j->pgid, in, out);
        else if (pid < 0)
          // Error when forking.
          error(EXIT_FAILURE, errno, "fork");
        else {
          // In parent process, set process id.
          p->pid = pid;

          // sleep for 15000 microseconds to avoid race condition
          // between processes in pipe. Then send SIGUSR1.
          usleep(15000);
          kill(pid, SIGUSR1);
        }
      }

      // Close input/output file descriptors.
      // Set consecutive pipe.
      if (in != j->stdin)
        close(in);
      if (out != j->stdout)
        close(out);
      in = mypipe[0];
    }

    // Pipe setting and processes launching finished,
    // wait for all the processes.
    for (p = j->head; p; p = p->next)
      if (p->pid >= 0) {
        if (isTimeX)
          // If it's a timeX job, hold the terminated processes
          // in zombie state for further handling.
          waitid(P_PID, p->pid, NULL, WEXITED | WNOWAIT);
        else
          waitpid(p->pid, &status, j->bg);
      }
  } else {
    // If the job does not involve piping or it's a timeX job.

    // If is not in timeX job, enable builtin commands.
    if (!isTimeX) {
      // Check if the command is a builtin keyword.
      builtin_func_ptr builtin = map(p->argv[0]);
      if (builtin != NULL)
        // Invoke builtin function if is builtin command.
        return builtin(p->argv, j);
    }

    // If in timeX job or is not a builtin command.
    pid = fork();
    if (pid == 0)
      // In child process, call launchProcess.
      launchProcess(p, j->pgid, j->stdin, j->stdout);
    else if (pid < 0)
      // Error when forking.
      error(EXIT_FAILURE, errno, "fork");
    else {
      // In parent process, set process id
      // and send SIGUSR1 to let child process run.
      p->pid = pid;
      kill(pid, SIGUSR1);

      if (isTimeX)
        // If it's a timeX job, hold the terminated process
        // in zombie state for further handling.
        waitid(P_PID, pid, NULL, WEXITED | WNOWAIT);
      else
        // Wait for child process if in foreground.
        waitpid(pid, &status, j->bg);
    }
  }

  // Clean up. Collect garbage.
  cleanJob(j);

  // Return the exit status.
  return WEXITSTATUS(status);
}

/*!
   \brief Clean up a \link Job job \endlink.

   \details
   This function releases the memory of job \p j and reset flag \c isTimeX.
   It releases the the process(es) and then releases the job itself.

   \param j Pointer to the job to be cleaned.

   \sa cleanProcess
*/
void cleanJob(Job* j) {
  isTimeX = 0;
  if (j == NULL)
    return;
  cleanProcess(j->head);
  free(j);
}

/*!
   \brief Clean up a \link Process process \endlink.

   \details
   This function releases the memory of process \p p recursively.
   It releases the argument vector and then releases the process itself.

   \param p Pointer to the process to be cleaned.
*/
void cleanProcess(Process* p) {
  if (p == NULL)
    return;
  cleanProcess(p->next);
  if (p->argv != NULL)
    free(p->argv);
  free(p);
}
