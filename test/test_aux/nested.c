#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int level, i;
	
	if (argc != 2) {
		printf("Usage: nested <no. of level>\n");
		exit(0);
	}
	
	level = atoi(argv[1]);
	
	for (i=0; i<level; i++) {
		if (fork() == 0) {
			continue;
		}else {
			sleep(1000);
			exit(0);
		}
	}
	return 0;
}