#include <stdio.h>
#include <unistd.h>

int main() {
  char c;

  do {
    c = getchar();
    printf("+%c", c);
  } while ((c != 'z') && (c != EOF));
  usleep(10000);	//sleep for 10 ms
  printf("\n");
  
  return 0;
}