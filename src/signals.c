#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "signals.h"

void intSighandler(int signum) {
  // TODO
}

void cldSighandler(int signum, siginfo_t* sig, void* context) {
  // TODO
}

void registerSighandler() {
  struct sigaction sa_int, sa_cld;
  sigaction(SIGINT, NULL, &sa_int);
  sa_int.sa_handler = intSighandler;
  sa_int.sa_flags &= ~SA_RESTART;
  sigaction(SIGINT, &sa_int, NULL);

  sigaction(SIGCHLD, NULL, &sa_cld);
  sa_cld.sa_flags |= SA_SIGINFO;
  sa_cld.sa_flags |= SA_RESTART;
  sa_cld.sa_sigaction = cldSighandler;
  sigaction(SIGCHLD, &sa_cld, NULL);
}
