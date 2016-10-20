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
   \fn int builtin_exit(char** argv, Job* j)
   \brief Builtin implementation for \c exit.

   \param argv Command argument vector.
   \param j Pointer to the current job.

   \return Execution status.
*/
int builtin_exit(char**, Job*);

/*!
   \fn int builtin_cd(char** argv, Job* j)
   \brief Builtin implementation for \c cd.

   \param argv Command argument vector.
   \param j Pointer to the current job.

   \return Execution status.
*/
int builtin_cd(char**, Job*);

/*!
   \fn int builtin_timeX(char** argv, Job* j)
   \brief Builtin implementation for \c timeX.

   \param argv Command argument vector.
   \param j Pointer to the current job.

   \return Execution status.
*/
int builtin_timeX(char**, Job*);

/*!
   \fn builtin_func_ptr map(const char* cmd)
   \brief Map keywords to corresponding functions.

   \param cmd The command string of the command line.

   \return The corresponding function pointer if \param cmd is a builtin keyword. Otherwise \c NULL.
*/
builtin_func_ptr map(const char*);

#endif
