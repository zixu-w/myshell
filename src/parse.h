#ifndef _PARSE_H
#define _PARSE_H

#include "command.h"
#include "jobs.h"

#define TOK_DELIMINATOR " \t\r\n\a"
#define CMD_DELIMINATOR "|"
#define ARGV_SIZE 30
#define CMD_SIZE 5

Job* parse(char*);

#endif
