#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "signals.h"

extern int flag;

void sighandler(int signum) {
  // TODO
}

void registerSighandler() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = sighandler;
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
}
