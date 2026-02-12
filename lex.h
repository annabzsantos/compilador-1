/**
 * @file lex.h
 * @author Prof. Ivairton M. Santos
 * @brief Modulo do analisador lexico
 * @version 0.4
 * @date 2022-03-16
 * 
 */

#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct_compiler.h"

// Prototipos
void initLex(char*);
type_token *getToken();
type_token *keyWordFind(char*);

#endif //_LEX_H_