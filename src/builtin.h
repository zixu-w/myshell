#ifndef _BUILTIN_H
#define _BUILTIN_H

#define ARGV_SIZE 30

#include "jobs.h"

typedef int(*builtin_func_ptr)(char**, Job*);

int builtin_exit(char**, Job*);
int builtin_cd(char**, Job*);
int builtin_timeX(char**, Job*);

builtin_func_ptr map(const char*);

#endif
