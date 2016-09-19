#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"

char** split(char* line) {
  char** tokens = (char**)malloc(ARGV_SIZE*sizeof(char*));
  if (tokens == NULL) {
    fprintf(stderr, "myshell: fail to allocate token buffer.\n");
    exit(EXIT_FAILURE);
  }
  char* token = strtok(line, DELIMINATOR);
  int pos = 0;
  while (token != NULL) {
    if (pos >= ARGV_SIZE) {
      fprintf(stderr, "myshell: token buffer overflow, max 30 tokens.\n");
      exit(EXIT_FAILURE);
    }
    tokens[pos++] = token;
    token = strtok(NULL, DELIMINATOR);
  }
  tokens[pos] = NULL;
  int i;
  for (i = 0; tokens[i] != NULL; ++i)
    printf("%s\n", tokens[i]);
  printf("%p\n", tokens);
  return tokens;
}

Command_t** parse(char** tokens) {
  Command_t** commands = (Command_t**)malloc(CMD_SIZE*sizeof(Command_t*));
  Command_t* command;
  command->cmd = tokens[0];
  command->bg_flag = 0;
  command->argv = tokens;
  printf("%p <=> %p\n", tokens, command->argv);
  printf("%p\n", tokens[0]);
  printf("%p\n", command->argv[1]);
  commands[0] = command;
  commands[1] = NULL;
  return commands;
}
