/*!
   \file signals.h
   \brief Header for signal-handling utilities.
   \author WANG Zixu
   \date 20/10/2016
*/

#ifndef _SIGNALS_H
#define _SIGNALS_H

#include <signal.h>
#include <sys/time.h>

/*!
   \var volatile sig_atomic_t sigur1Received
   \brief Signal flag for \c SIGUSR1.

   \details
   This volatile variable is used for blocking child processes
   until parent process sends \c SIGUSR1.
*/
volatile sig_atomic_t sigur1Received;

/*!
   \var volatile sig_atomic_t isTimeX
   \brief Signal flag for \c timeX jobs.

   \details
   This volatile variable is used for indicating \c timeX jobs.
*/
volatile sig_atomic_t isTimeX;

/*!
   \var struct timeval startTime
   \brief Start time stamp.

   \details
   Record start time stamp for \c timeX jobs to calculate real time.
*/
struct timeval startTime,

/*!
   \var struct timeval endTime
   \brief End time stamp.

   \details
   Record end time stamp for \c timeX jobs to calculate real time.
*/
               endTime;

/*!
  \fn void registerSighandler(void)
  \brief Register signal handlers.
*/
void registerSighandler(void);

#endif
