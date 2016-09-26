#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "signals.h"

void sighandler(int signum) {
  // TODO
}

void registerSighandler() {
  struct sigaction sa;
  sigaction(SIGINT, NULL, &sa);
  sa.sa_handler = sighandler;
  sa.sa_flags &= ~SA_RESTART;
  sigaction(SIGINT, &sa, NULL);
}
