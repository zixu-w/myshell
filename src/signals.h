#ifndef _SIGNALS_H
#define _SIGNALS_H

#include <signal.h>

volatile sig_atomic_t sigur1Received;

void registerSighandler(void);

#endif
