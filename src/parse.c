/*!
   \file parse.c
   \brief Parse input command lines into \link Job jobs \endlink .
   \author WANG Zixu
   \date 20/10/2016
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"

/*!
   \brief Parse the command line.

   \details
   This function parses the input command line and generates a corresponding
   job if it's valid.
   It uses a finite state machine to parse commands and special grammer token
   such as \c '|' for piping and \c '&' for background.

   \param line String of the input command line read from readline.

   \return Pointer to the generated job. \c NULL if the input is illegal.
*/
Job* parse(char* line) {
  // If the input is empty, return NULL.
  if (line == NULL || strcmp(line, "") == 0)
    return NULL;

  // Allocate job buffer.
  Job* j = (Job*)malloc(sizeof(Job));
  if (j == NULL) {
    fprintf(stderr, "myshell: fail to allocate job buffer.\n");
    return NULL;
  }
  memset(j, 0, sizeof(Job));

  // Initialize job I/O file descriptors.
  j->stdin = STDIN_FILENO;
  j->stdout = STDOUT_FILENO;

  // Define states for the finite state machine.
  enum _STATE {
    CMD_INI,  // Command initialization.
    TOK_INI,  // Token initialization.
    PRE_TOK,  // Pre-token processing.
    TOK,      // Read token.
    POST_TOK, // Post-token processing.
    CMD_FIN,  // Command finished.
    PIP,      // Piping.
    BKG,      // Background.
  } state = CMD_INI; // Initialize the state to CMD_INI.

  char* token;
  char* tok;
  char** cmds;
  Process* head = NULL;
  Process* prev = NULL;
  Process* p;
  size_t argSize, pos;
  int delta = 1;

  // Processing loop of the finite state machine.
  for (token = line; *token; token+=delta /* Control step of loop. */) {
    // Reset step to one character.
    delta = 1;

    // In state CMD_INI.
    if (state == CMD_INI) {
      // Allocate commands buffer.
      cmds = (char**)malloc((ARG_SIZE+1)*sizeof(char*));
      if (cmds == NULL) {
        fprintf(stderr, "myshell: fail to allocate command buffer.\n");
        cleanJob(j);
        return NULL;
      }
      memset(cmds, 0, (ARG_SIZE+1)*sizeof(char*));

      // Allocate process buffer.
      p = (Process*)malloc(sizeof(Process));
      if (p == NULL) {
        fprintf(stderr, "myshell: fail to allocate process buffer.\n");
        cleanJob(j);
        free(cmds);
        return NULL;
      }
      memset(p, 0, sizeof(Process));
      argSize = 0;

      // Pre-command initialization completed.
      // System transfers to TOK_INI state.
      state = TOK_INI;
    }

    // In TOK_INI state.
    if (state == TOK_INI) {
      // Initialize iterator.
      pos = 0;

      // Allocate token buffer.
      tok = (char*)malloc((TOK_SIZE+1)*sizeof(char));
      if (tok == NULL) {
        fprintf(stderr, "myshell: fail to allocate token buffer.\n");
        cleanJob(j);
        free(p);
        free(cmds);
        return NULL;
      }
      memset(tok, 0, (TOK_SIZE+1)*sizeof(char));

      // If the character read is whitespace, transfer to PRE_TOK state.
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a') {
        state = PRE_TOK;
        continue;
      } else if (*token == '|' || *token == '&') {
        // If the character read is piping or background indicator, raise error.
        fprintf(stderr, "myshell: parse error near '%c'.\n", *token);
        cleanJob(j);
        free(p);
        free(tok);
        free(cmds);
        return NULL;
      } else
        // Otherwise, transfer to TOK state to read token.
        state = TOK;
    }

    // In state PRE_TOK.
    if (state == PRE_TOK) {
      // Skip whitespace characters.
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        continue;

      // If the character read is piping or background indicator, raise error.
      else if (*token == '|' || *token == '&') {
        fprintf(stderr, "myshell: parse error near '%c'.\n", *token);
        cleanJob(j);
        free(p);
        free(tok);
        free(cmds);
        return NULL;
      } else
        // Otherwise, transfer to TOK state to read token.
        state = TOK;
    }

    // In state TOK.
    if (state == TOK) {
      // The character read is whitespace indicates that the token is finished,
      // transfer to POST_TOK state.
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a' || *token == '|' || *token == '&')
        state = POST_TOK;

      // Read the token.
      else {
        tok[pos++] = *token;

        // If reach the end of command line, transfer to POST_TOK state.
        if (*(token+1) == '\0') {
          token++;
          state = POST_TOK;
        }
      }
    }

    // In state POST_TOK.
    if (state == POST_TOK) {
      // Strip trailing whitespace.
      if (*(token+1) == '\0' && *token == ' ')
        token++;
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        continue;
      else {
        // Null terminate the token string.
        tok[pos] = '\0';

        // If exceeds argument number limit, raise error.
        if (argSize >= ARG_SIZE) {
          fprintf(stderr, "myshell: arguments buffer overflow, max %d arguments.\n", ARG_SIZE);
          cleanJob(j);
          free(p);
          free(tok);
          free(cmds);
          return NULL;
        }

        // Append token to command.
        cmds[argSize++] = tok;

        // Check for command finish.
        if (*token == '|' || *token == '&' || *token == '\0')
          state = CMD_FIN;
        else {
          // Hold the current character and transfer to TOK_INI
          // to process next token.
          delta = 0;
          state = TOK_INI;
        }
      }
    }

    // In state CMD_FIN.
    if (state == CMD_FIN) {
      // Allocate argument vector buffer in process.
      p->argv = (char**)malloc((argSize+1)*sizeof(char*));
      if (p->argv == NULL) {
        fprintf(stderr, "myshell: fail to allocate argv buffer.\n");
        cleanJob(j);
        free(p);
        free(tok);
        free(cmds);
        return NULL;
      }
      memset(p->argv, 0, (argSize+1)*sizeof(char*));

      // Copy arguments.
      size_t i;
      for (i = 0; i < argSize; ++i)
        p->argv[i] = cmds[i];
      p->argv[argSize] = NULL;

      // Connect pipe.
      if (head == NULL) {
        head = p;
        prev = p;
      } else {
        prev->next = p;
        prev = p;
      }

      // If indicated background, transfer to BKG.
      if (*token == '&')
        state = BKG;

      // If indicated piping, transfer to PIP.
      else if (*token == '|')
        state = PIP;

      // Command finished, transfer to CMD_INI to parse next command.
      else {
        state = CMD_INI;
        continue;
      }
    }

    // In state PIP.
    if (state == PIP) {
      // Hold the current character.
      delta = 0;

      // If reach end of command line, raise error.
      if (*(++token) == '\0') {
        fprintf(stderr, "myshell: parse error near '|'.\n");
        cleanJob(j);
        free(tok);
        free(cmds);
        return NULL;
      }

      // Skip whitespace.
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        continue;

      // Transfer to CMD_INI to parse the command after the pipe.
      else {
        state = CMD_INI;
        continue;
      }
    }

    // In state BKG.
    if (state == BKG) {
      // Set job background flag.
      j->bg = 1;

      // End parsing if reach end.
      if (*(++token) == '\0')
        break;

      // Skip whitespace.
      if (*token == ' ' || *token == '\t' || *token == '\r' || *token == '\a')
        continue;

      // If other characters appear, raise error.
      else {
        fprintf(stderr, "myshell: '&' should not appear in the middle of the command line\n");
        cleanJob(j);
        free(tok);
        free(cmds);
        return NULL;
      }
    }
  }

  // Assign parsed process(es).
  j->head = head;

  // If no process is generated, clean up and return NULL.
  if (j->head == NULL) {
    cleanJob(j);
    return NULL;
  }

  // Return generated job.
  return j;
}
