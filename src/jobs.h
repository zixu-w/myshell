/*!
   \file jobs.h
   \brief Job and \link Process process \endlink structure definition.
   \author WANG Zixu
   \date 20/10/2016
*/

#ifndef _JOBS_H
#define _JOBS_H

#include <unistd.h>

/*!
   \struct Process
   \brief Definition of process structure.

   \details
   A \c Process structure represents a created/running/terminated process in the
   system. It stores its arguments in \c argv, its process id in \c pid, and
   next process pointed by \c next if there is a pipe related to the process.
*/
typedef struct Process {
  struct Process* next; /*!< Pointer to the next process in a pipe. */
  char** argv; /*!< Argument vector of the process. */
  pid_t pid; /*!< Pid of the process */
} Process;

/*!
   \struct Job
   \brief Definition of job structure.

   \details
   A \c Job structure represents a command line inputed by the user. It stores
   the \link Process Process(es) \endlink of the job in a link list \c head,
   the group pid in \c pgid, input and output file descriptors in \c stdin and
   \c stdout, and background indicator in \c bg.
*/
typedef struct Job {
  Process* head; /*!< Head of the process link list. */
  pid_t pgid; /*!< Group pid. */
  int stdin, /*!< Input file descriptor. */
      stdout; /*!< Output file descriptor. */
  int bg; /*!< Background indicator. 1 for running in background. */
} Job;

/*!
   \fn int launchJob(Job*)
   \brief Launch a \link Job job \endlink.
*/
int launchJob(Job*);

/*!
   \fn void cleanJob(Job*)
   \brief Clean up a \link Job job \endlink.
*/
void cleanJob(Job*);

/*!
   \fn void cleanProcess(Process*)
   \brief Clean up a \link Process process \endlink.
*/
void cleanProcess(Process*);

#endif
