/*!
   \file parse.h
   \brief Header for parsing utilities.
   \author WANG Zixu
   \date 20/10/2016
*/

#ifndef _PARSE_H
#define _PARSE_H

#include "jobs.h"

/*!
   \def TOK_SIZE
   \brief Marco definition for token vector size.
*/
#define TOK_SIZE 30

/*!
   \def ARG_SIZE
   \brief Marco definition for argument length.
*/
#define ARG_SIZE 30

/*!
   \fn Job* parse(char*)
   \brief Parse the command line.
*/
Job* parse(char*);

#endif
