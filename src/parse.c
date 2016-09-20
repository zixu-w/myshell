#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"

int pos;

char** split(char* line) {
  char** tokens = (char**)malloc(ARGV_SIZE*sizeof(char*));
  if (tokens == NULL) {
    fprintf(stderr, "myshell: fail to allocate token buffer.\n");
    exit(EXIT_FAILURE);
  }
  char* saveptr;
  char* token = strtok_r(line, DELIMINATOR, &saveptr);
  pos = 0;
  while (token != NULL) {
    if (pos >= ARGV_SIZE) {
      fprintf(stderr, "myshell: token buffer overflow, max 30 tokens.\n");
      exit(EXIT_FAILURE);
    }
    tokens[pos++] = token;
    token = strtok_r(NULL, DELIMINATOR, &saveptr);
  }
  tokens[pos] = NULL;
  return tokens;
}

Command_t** parse(char** tokens) {
  Command_t** commands = (Command_t**)malloc(CMD_SIZE*sizeof(Command_t*));
  Command_t command;
  memset(&command, 0, sizeof(Command_t));
  command.cmd = tokens[0];
  if (strcmp(tokens[pos-1], "&") == 0) {
    tokens[--pos] = NULL;
    command.bg_flag = 1;
  } else
    command.bg_flag = 0;
  command.argv = tokens;
  commands[0] = &command;
  commands[1] = NULL;
  return commands;
}
