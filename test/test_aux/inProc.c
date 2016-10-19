#include <stdio.h>
#include <unistd.h>

int main() {

  char str[27] = "abcdefghijklmnopqrstuvwxyz";
  short i;
  for (i=0; i<26; i++) {
    printf("%c", str[i]);
	usleep(1000);	//sleep for 1ms
  } 
  
  return 0;
}