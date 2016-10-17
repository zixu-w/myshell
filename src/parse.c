#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"

Job* parse(char* line) {
  if (line == NULL || strcmp(line, "") == 0)
    return NULL;
  Job* j = (Job*)malloc(sizeof(Job));
  if (j == NULL) {
    fprintf(stderr, "myshell: fail to allocate job buffer.\n");
    return NULL;
  }
  memset(j, 0, sizeof(Job));
  j->stdin = STDIN_FILENO;
  j->stdout = STDOUT_FILENO;
  
  enum _STATE {INI, CMD, WHS, PIP, BKG} state = INI;
  char* token;
  char* tok;
  char** cmds;
  Process* head;
  Process* prev;
  Process* p;
  size_t argSize, pos;
  for (token = line; *token; ++token) {
    if (state == INI) {
      tok = (char*)malloc((TOK_SIZE+1)*sizeof(char));
      if (tok == NULL) {
        fprintf(stderr, "myshell: fail to allocate token buffer.\n");
        free(j);
        return NULL;
      }
      memset(tok, 0, (TOK_SIZE+1)*sizeof(char));
      cmds = (char**)malloc((ARG_SIZE+1)*sizeof(char*));
      if (cmds == NULL) {
        fprintf(stderr, "myshell: fail to allocate command buffer.\n");
        free(j);
        free(tok);
        return NULL;
      }
      memset(cmds, 0, (ARG_SIZE+1)*sizeof(char*));
      p = (Process*)malloc(sizeof(Process));
      if (p == NULL) {
        fprintf(stderr, "myshell: fail to allocate process buffer.\n");
        free(j);
        free(tok);
        free(cmds);
        return NULL;
      }
      memset(p, 0, sizeof(Process));
      head = NULL;
      prev = NULL;
      argSize = 0;
      pos = 0;
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        state = WHS;
      else if (*token == '|' || *token == '&') {
        fprintf(stderr, "myshell: parse error near '%c'.\n", *token);
        free(j);
        free(p);
        free(tok);
        free(cmds);
        return NULL;
      } else
        state = CMD;
    }

    if (state == WHS) {
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        continue;
      else
        state = CMD;
    }

    if (state == CMD) {
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a') {
        tok[pos] = '\0';
        if (argSize >= ARG_SIZE) {
          fprintf(stderr, "myshell: arguments buffer overflow, max %d arguments.\n", ARG_SIZE);
          free(j);
          free(p);
          free(tok);
          free(cmds);
          return NULL;
        }
        cmds[argSize++] = tok;
        tok = (char*)malloc((TOK_SIZE+1)*sizeof(char));
        pos = 0;
        state = WHS;
      }
      else if (*token == '|' || *token == '&') {
        p->argv = (char**)malloc((argSize+1)*sizeof(char*));
        size_t i;
        for (i = 0; i < argSize; ++i)
          p->argv[i] = cmds[i];
        p->argv[argSize] = NULL;
        if (head == NULL) {
          head = p;
          prev = p;
        } else {
          prev->next = p;
          prev = p;
        }
        if (*token == '|')
          state = INI;
        else if (*token == '&')
          state = BKG;
        continue;
      } else {
        tok[pos++] = *token;
        if (*(token+1) == '\0') {
          tok[pos] = '\0';
          if (argSize >= ARG_SIZE) {
            fprintf(stderr, "myshell: arguments buffer overflow, max %d arguments.\n", ARG_SIZE);
            free(j);
            free(p);
            free(tok);
            free(cmds);
            return NULL;
          }
          cmds[argSize++] = tok;
          p->argv = (char**)malloc((argSize+1)*sizeof(char*));
          size_t i;
          for (i = 0; i < argSize; ++i)
            p->argv[i] = cmds[i];
          p->argv[argSize] = NULL;
          if (head == NULL) {
            head = p;
            prev = p;
          } else {
            prev->next = p;
            prev = p;
          }
        }
      }
    }

    if (state == BKG) {
      j->bg = 1;
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        continue;
      else {
        fprintf(stderr, "myshell: '&' should not appear in the middle of the command line\n");
        free(j);
        free(p);
        free(tok);
        free(cmds);
        return NULL;
      }
    }
  }
  j->head = head;
  if (j->head == NULL)
    return NULL;
  return j;
}
