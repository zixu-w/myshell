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

  enum _STATE {
    CMD_INI,
    TOK_INI,
    PRE_TOK,
    TOK,
    POST_TOK,
    CMD_FIN,
    BKG,
  } state = CMD_INI;
  char* token;
  char* tok;
  char** cmds;
  Process* head = NULL;
  Process* prev = NULL;
  Process* p;
  size_t argSize, pos;
  int delta = 1;
  for (token = line; *token; token+=delta) {
    delta = 1;
    if (state == CMD_INI) {
      cmds = (char**)malloc((ARG_SIZE+1)*sizeof(char*));
      if (cmds == NULL) {
        fprintf(stderr, "myshell: fail to allocate command buffer.\n");
        free(j);
        return NULL;
      }
      memset(cmds, 0, (ARG_SIZE+1)*sizeof(char*));
      p = (Process*)malloc(sizeof(Process));
      if (p == NULL) {
        fprintf(stderr, "myshell: fail to allocate process buffer.\n");
        free(j);
        free(cmds);
        return NULL;
      }
      memset(p, 0, sizeof(Process));
      argSize = 0;
      state = TOK_INI;
    }

    if (state == TOK_INI) {
      pos = 0;
      tok = (char*)malloc((TOK_SIZE+1)*sizeof(char));
      if (tok == NULL) {
        fprintf(stderr, "myshell: fail to allocate token buffer.\n");
        free(j);
        free(p);
        free(cmds);
        return NULL;
      }
      memset(tok, 0, (TOK_SIZE+1)*sizeof(char));
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a') {
        state = PRE_TOK;
        continue;
      } else if (*token == '|' || *token == '&') {
        fprintf(stderr, "myshell: parse error near '%c'.\n", *token);
        free(j);
        free(p);
        free(tok);
        free(cmds);
        return NULL;
      } else
        state = TOK;
    }

    if (state == PRE_TOK) {
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        continue;
      else if (*token == '|' || *token == '&') {
        fprintf(stderr, "myshell: parse error near '%c'.\n", *token);
        free(j);
        free(p);
        free(tok);
        free(cmds);
        return NULL;
      } else
        state = TOK;
    }

    if (state == TOK) {
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        state = POST_TOK;
      else if (*token == '|' || *token == '&') {
        state = CMD_FIN;
      } else {
        tok[pos++] = *token;
        if (*(token+1) == '\0') {
          token++;
          state = POST_TOK;
        }
      }
    }

    if (state == POST_TOK) {
      if (*(token+1) == '\0')
        token++;
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        continue;
      else {
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
        if (*token == '|' || *token == '&' || *token == '\0')
          state = CMD_FIN;
        else {
          delta = 0;
          state = TOK_INI;
        }
      }
    }

    if (state == CMD_FIN) {
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
      if (*token == '&')
        state = BKG;
      else {
        state = CMD_INI;
        continue;
      }
    }

    if (state == BKG) {
      j->bg = 1;
      if (*(++token) == '\0')
        break;
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
