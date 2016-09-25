#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
  FILE* in = fopen("test/test.in", "r");
  FILE* out = fopen("test/test.out", "w");
  if (in == NULL || out == NULL) {
    perror("File failed");
    exit(EXIT_FAILURE);
  }
  int infd = fileno(in);
  int outfd = fileno(out);
  pid_t pid = fork();
  if (pid == 0) {
    dup2(infd, 0);
    dup2(outfd, 1);
    dup2(outfd, 2);
    close(infd);
    close(outfd);
    char* argv[3] = {"bin/myshell", "--echo", 0};
    if (execvp(argv[0], argv) == -1)
      perror("Exec failed");
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("Fork failed");
    exit(EXIT_FAILURE);
  } else {
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
  }
}
