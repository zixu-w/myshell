#include <stdlib.h>
#include <stdio.h>
#include <signal.h>		/* To handle various signals */


volatile int timer = 0;

void sigint_handler(int signum) {
	
#if FOREVER == 1
	printf("Receives SIGINT!! IGNORE IT :)\n");
#else
	exit(0);
#endif
}

void sigalrm_handler(int signum) {
	
	printf("Time is up: %d seconds\nProgram terminated.\n", timer);
	exit(0);
}

int main(int argc, char ** argv) {
	unsigned long long i, x;
	struct sigaction sa;

	/* Install signal handler for SIGINT */
	/* For terminating any existing foreground process */
	sigaction(SIGINT, NULL, &sa);
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);
	
	/* Install signal handler for SIGALRM */
	sigaction(SIGALRM, NULL, &sa);
	sa.sa_handler = sigalrm_handler;
	sigaction(SIGALRM, &sa, NULL);
	
	if (argc > 2) {
		printf("Usage: ./loopf [second]\n");
		exit(0);
	} else if (argc == 2) {
		timer = atoi(argv[1]);
		alarm(timer);
	}

	x = 1;
	while (1) {
		for (i=0; i<1073741824; i += 2)
			x *= i;
			if (x > 8589934589)
				x = 1;
	}	
	return 0;
}
