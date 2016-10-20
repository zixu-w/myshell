/*!
   \file builtin.h
   \brief Header for builtin functionalities.
   \author WANG Zixu
   \date 20/10/2016
*/

#ifndef _BUILTIN_H
#define _BUILTIN_H

/*!
   \def ARGV_SIZE
   \brief Marco definition for argument vector size.
*/
#define ARGV_SIZE 30

#include "jobs.h"

/*!
   \var typedef int(*builtin_func_ptr)(char**, Job*)
   \brief Type definition of builtin function pointer.
*/
typedef int(*builtin_func_ptr)(char**, Job*);

/*!
   \fn int builtin_exit(char**, Job*)
   \brief Builtin implementation for \c exit.
*/
int builtin_exit(char**, Job*);

/*!
   \fn int builtin_cd(char**, Job*)
   \brief Builtin implementation for \c cd.
*/
int builtin_cd(char**, Job*);

/*!
   \fn int builtin_timeX(char**, Job*)
   \brief Builtin implementation for \c timeX.
*/
int builtin_timeX(char**, Job*);

/*!
   \fn builtin_func_ptr map(const char*)
   \brief Map keywords to corresponding functions.
*/
builtin_func_ptr map(const char*);

#endif
