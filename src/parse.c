#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"

Job* parse(char* line) {
  if (line == NULL || strcmp(line, "") == 0)
    return NULL;
  Job* j = (Job*)malloc(sizeof(Job));
  memset(j, 0, sizeof(Job));
  j->stdin = STDIN_FILENO;
  j->stdout = STDOUT_FILENO;
  char** cmds = (char**)malloc((CMD_SIZE+1)*sizeof(char*));
  if (cmds == NULL) {
    fprintf(stderr, "myshell: fail to allocate command buffer.\n");
    free(j);
    return NULL;
  }
  int cmdSize = 0;
  char* cmd = strtok(line, CMD_DELIMINATOR);
  while (cmd != NULL) {
    if (cmdSize >= CMD_SIZE) {
      fprintf(stderr, "myshell: command buffer overflow, max %d commands.\n", CMD_SIZE);
      free(j);
      free(cmds);
      return NULL;
    }
    cmds[cmdSize++] = cmd;
    cmd = strtok(NULL, CMD_DELIMINATOR);
  }
  cmds[cmdSize] = NULL;
  Process* head = NULL;
  Process* prev = NULL;
  int i;
  for (i = 0; i < cmdSize; ++i) {
    cmd = cmds[i];
    char** tokens = (char**)malloc((ARGV_SIZE+1)*sizeof(char*));
    if (tokens == NULL) {
      fprintf(stderr, "myshell: fail to allocate token buffer.\n");
      free(j);
      free(cmds);
      return NULL;
    }
    int tokSize = 0;
    char* token = strtok(cmd, TOK_DELIMINATOR);
    while (token != NULL) {
      if (tokSize >= ARGV_SIZE) {
        fprintf(stderr, "myshell: token buffer overflow, max %d tokens.\n", ARGV_SIZE);
        free(j);
        free(cmds);
        free(tokens);
        return NULL;
      }
      tokens[tokSize++] = token;
      token = strtok(NULL, TOK_DELIMINATOR);
    }
    if (tokSize >= 1 && strcmp(tokens[tokSize-1], "&") == 0) {
      if (cmds[i+1] != NULL) {
        fprintf(stderr, "myshell: '&' should not appear in the middle of the command line\n");
        free(j);
        free(cmds);
        free(tokens);
        return NULL;
      }
      j->bg = WNOHANG;
      tokens[--tokSize] = NULL;
    } else
      tokens[tokSize] = NULL;
    Process* p = (Process*)malloc(sizeof(Process));
    memset(p, 0, sizeof(Process));
    p->argv = (char**)malloc((tokSize+1)*sizeof(char*));
    int j;
    for (j = 0; j < tokSize; ++j)
      p->argv[j] = tokens[j];
    p->argv[tokSize] = NULL;
    if (head == NULL) {
      head = p;
      prev = p;
    } else {
      prev->next = p;
      prev = p;
    }
  }
  j->head = head;
  return j;
}
