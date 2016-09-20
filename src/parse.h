#ifndef _PARSE_H
#define _PARSE_H

#include "command.h"

#define DELIMINATOR " \t\r\n\a"
#define ARGV_SIZE 30
#define CMD_SIZE 5

char** split(char*);
Command_t** parse(char**);

#endif
