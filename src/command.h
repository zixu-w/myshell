#ifndef _COMMAND_H
#define _COMMAND_H

typedef struct Command_t {
  char* cmd;
  int bg_flag;
  char** argv;
} Command_t;

#endif
