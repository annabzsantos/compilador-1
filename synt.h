/**
 * @file synt.h
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Modulo do analisador sintatico
 * @version 0.6
 * @date 2022-03-17
 *
 */

#ifndef _SYNT_H_
#define _SYNT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct_compiler.h"
#include "lex.h"
#include "gen.h"
#include "symbols.h"

/*
 * Gramatica:
 *
 * program          -> declarations begin statements end func_code
 * declarations     -> declaration declarations | <vazio>
 * declaration      -> {int|float|string|char} id { DeclaracaoV | DeclaracaoF }
 * DeclaracaoV      -> ;
 * DeclaracaoF      -> ( [{tipo} id ,]* [{tipo} id] | <vazio> ) ;
 * statements       -> statement statements | <vazio>
 * statement        -> read_stmt
 *                   | write_stmt
 *                   | if_stmt [ else_stmt ]
 *                   | while_stmt
 *                   | id = E ;          (atribuicao)
 *                   | func_call_cmd     (chamada de funcao)
 * func_call_cmd    -> id ( [id {, id}*] | <vazio> ) ;
 * func_code        -> func_implementation func_code | <vazio>
 * func_implementation -> {tipo} id ( [{tipo} id {, {tipo} id}*] ) begin statements end
 */

// Prototipos
void program(void);
void declarations(void);
int  declaration(void);
void statements(void);
int  statement(void);
int  func_call_cmd(char *func_name);  // Regra de derivacao para chamada de funcao
void func_code(void);
int  func_implementation(void);
int  B(void);
int  boolOperator(int*);
int  E(void);
int  ER(void);
int  T(void);
int  TR(void);
int  F(void);
int  match(int);
//int  main(void);

#endif //_SYNT_H_