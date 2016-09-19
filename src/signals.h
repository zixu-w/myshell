#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <signal.h>

int flag;

void sighandler(int);
void registerSighandler(void);

#endif
