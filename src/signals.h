#ifndef _SIGNALS_H
#define _SIGNALS_H

#include <signal.h>
#include <sys/time.h>

volatile sig_atomic_t sigur1Received;
volatile sig_atomic_t isTimeX;
struct timeval startTime, endTime;


void registerSighandler(void);

#endif
