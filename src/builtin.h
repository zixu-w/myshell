#ifndef _BUILTIN_H
#define _BUILTIN_H

#define ARGV_SIZE 30

typedef int(*builtin_func_ptr)(char**);

int builtin_exit(char**);
int builtin_cd(char**);

builtin_func_ptr map(const char*);

#endif
