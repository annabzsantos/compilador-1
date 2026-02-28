/**
 * @file synt.c
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Codificacao do modulo do analisador sintatico
 * @version 0.5
 * @date 2022-02-04
 *
 */

#include "synt.h"
#include <stdbool.h>

// Variaveis globais
type_token *lookahead;
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
extern char output_file_name[MAX_CHAR];
extern FILE *output_file;

/**
 * @brief Verifica se o proximo token na cadeia eh o esperado
 */
int match(int token_tag) {
    if (lookahead->tag == token_tag) {
        lookahead = getToken();
        return true;
    }
    printf("[ERRO] Entrada esperada: %s\n", lookahead->lexema);
    return false;
}

/**
 * @brief Regra de derivacao inicial
 * Programa -> Declaracoes begin Comandos end Func_code
 */
void program(void) {
    gen_preambule();
    gen_preambule_code();
    declarations();

    match(BEGIN);
    statements();
    match(END);

    gen_epilog_code();  // Encerra o programa principal
    func_code();        // Gera o codigo das funcoes (apos o principal)
    gen_data_section(); // Gera a secao de dados

    printSTFunctions();
}

/**
 * @brief Regra de derivacao para declaracoes
 * Declaracoes -> Declaracao | <vazio>
 */
void declarations(void) {
    while (declaration());
}

/**
 * @brief Regra de derivacao declaracao
 * Declaracao -> {int|float|string|char} id { DeclaracaoV | DeclaracaoF }
 * DeclaracaoV -> ;
 * DeclaracaoF -> ( [{tipo} id ,]* [{tipo} id] | <vazio> ) ;
 * @return int true/false
 */
int declaration(void) {
    int type = lookahead->tag;
    if (type != INT && type != FLOAT && type != CHAR && type != STRING) {
        return false;
    }

    int return_type = type;
    match(type);

    char name[MAX_CHAR];
    strcpy(name, lookahead->lexema);
    match(ID);

    // DeclaracaoV -> ;
    if (lookahead->tag == SEMICOLON) {
        if (sym_find(name, &global_symbol_table_variables) != NULL) {
            printf("[ERRO] Variavel '%s' ja declarada.\n", name);
            return false;
        }
        sym_declare(name, type, 0, &global_symbol_table_variables);
        return match(SEMICOLON);

    // DeclaracaoF -> ( ... ) ;
    } else if (lookahead->tag == OPEN_PAR) {
        if (sym_func_find(name) != NULL) {
            printf("[ERRO] Funcao '%s' ja declarada.\n", name);
            return false;
        }

        match(OPEN_PAR);

        type_param params[MAX_PARAMS];
        int nparams = 0;

        while (lookahead->tag != CLOSE_PAR) {
            int param_type = lookahead->tag;
            if (param_type != INT && param_type != FLOAT && param_type != CHAR && param_type != STRING) {
                printf("[ERRO] Tipo de parametro invalido na declaracao de '%s'.\n", name);
                return false;
            }
            match(param_type);

            char param_name[MAX_CHAR];
            strcpy(param_name, lookahead->lexema);
            match(ID);

            // Declara parametro como variavel global (se ainda nao existir)
            if (sym_find(param_name, &global_symbol_table_variables) == NULL) {
                sym_declare(param_name, param_type, 0, &global_symbol_table_variables);
            }

            params[nparams].type = param_type;
            strcpy(params[nparams].name, param_name);
            nparams++;

            if (lookahead->tag == COMMA) {
                match(COMMA);
            }
        }

        match(CLOSE_PAR);
        match(SEMICOLON);

        // Registra a funcao na TSF (com label gerado automaticamente)
        sym_func_declare(name, return_type, params, nparams);
        return true;

    } else {
        printf("[ERRO] Esperado ';' ou '(' apos identificador '%s'.\n", name);
        return false;
    }
}

/**
 * @brief Regra de derivacao para comandos
 */
void statements(void) {
    while (statement());
}

/**
 * @brief Regra de derivacao que processa os comandos
 * statement -> read_stmt | write_stmt | if_stmt | while_stmt | assign_stmt | func_call_cmd
 * @return int true/false
 */
int statement(void) {
    char lexeme_of_id[MAX_CHAR];
    type_symbol_table_entry *search_symbol;
    type_symbol_table_string_entry *gen_string;
    int ok1, ok2, type;
    char string_value[MAX_CHAR];

    // read id ;
    if (lookahead->tag == READ) {
        match(READ);
        strcpy(lexeme_of_id, lookahead->lexema);
        ok1 = match(ID);
        search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
        if (search_symbol != NULL) {
            type = search_symbol->type;
            gen_read(lexeme_of_id, type);
            ok2 = match(SEMICOLON);
            return ok1 && ok2;
        } else {
            printf("[ERRO] Variavel nao declarada: %s\n", lexeme_of_id);
            return false;
        }

    // write id | "string" ;
    } else if (lookahead->tag == WRITE) {
        match(WRITE);
        if (lookahead->tag == STRING) {
            strcpy(string_value, lookahead->lexema);
            gen_string = sym_string_declare(string_value);
            match(STRING);
            if (gen_string != NULL) {
                strcpy(lexeme_of_id, gen_string->name);
                gen_write(lexeme_of_id, STRING);
            }
            match(SEMICOLON);
            return true;
        } else if (lookahead->tag == ID) {
            strcpy(lexeme_of_id, lookahead->lexema);
            match(ID);
            search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
            if (search_symbol != NULL) {
                type = search_symbol->type;
                gen_write(lexeme_of_id, type);
                match(SEMICOLON);
                return true;
            } else {
                printf("[ERRO] Variavel nao declarada: %s\n", lexeme_of_id);
                return false;
            }
        }

    // if ( B ) begin statements end [ else begin statements end ]
    } else if (lookahead->tag == IF) {
        char label_else[MAX_CHAR];
        char label_end[MAX_CHAR];
        gen_label_name(label_else);
        gen_label_name(label_end);

        match(IF);
        match(OPEN_PAR);
        B();
        gen_cond_jump(label_else);
        match(CLOSE_PAR);

        match(BEGIN);
        statements();
        match(END);
        gen_incond_jump(label_end);
        gen_label(label_else);

        if (lookahead->tag == ELSE) {
            match(ELSE);
            match(BEGIN);
            statements();
            match(END);
        }
        gen_label(label_end);
        return true;

    // while ( B ) begin statements end
    } else if (lookahead->tag == WHILE) {
        char label_begin[MAX_CHAR];
        char label_end[MAX_CHAR];
        gen_label_name(label_begin);
        gen_label_name(label_end);

        match(WHILE);
        match(OPEN_PAR);
        gen_label(label_begin);
        B();
        gen_cond_jump(label_end);
        match(CLOSE_PAR);

        match(BEGIN);
        statements();
        match(END);
        gen_incond_jump(label_begin);
        gen_label(label_end);
        return true;

    // id = E ;        (atribuicao)
    // id ( [...] ) ;  (chamada de funcao) — resolve ambiguidade pelo lookahead apos o ID
    } else if (lookahead->tag == ID) {
        strcpy(lexeme_of_id, lookahead->lexema);
        match(ID);

        // Atribuicao: id = E ;
        if (lookahead->tag == ASSIGN) {
            search_symbol = sym_find(lexeme_of_id, &global_symbol_table_variables);
            if (search_symbol == NULL) {
                printf("[ERRO] Variavel nao declarada: %s\n", lexeme_of_id);
                return false;
            }
            match(ASSIGN);
            if (!E()) return false;
            gen_assign(lexeme_of_id);
            return match(SEMICOLON);

        // Chamada de funcao: id ( [...] ) ;
        } else if (lookahead->tag == OPEN_PAR) {
            // Delega para a regra de derivacao func_call_cmd,
            // passando o nome da funcao ja consumido pelo lookahead
            return func_call_cmd(lexeme_of_id);

        } else {
            printf("[ERRO] Esperado '=' ou '(' apos identificador '%s'.\n", lexeme_of_id);
            return false;
        }

    } else if (lookahead->tag == ENDTOKEN || lookahead->tag == END) {
        return false;

    } else {
        printf("[ERRO] Comando desconhecido. Tag=%d; Lexema=%s\n",
               lookahead->tag, lookahead->lexema);
        return false;
    }

    return false;
}

/**
 * @brief Regra de derivacao para chamada de funcao
 *
 * func_call_cmd -> id ( [id {, id}*] | <vazio> ) ;
 *
 * Verificacoes realizadas:
 *   1. O id da funcao esta presente na TSF?
 *   2. A quantidade de argumentos e equivalente ao prototipo?
 *   3. Cada argumento tem o mesmo nome e tipo que o parametro prototipado?
 *
 * @param func_name  nome da funcao ja consumido em statement()
 * @return int true/false
 */
int func_call_cmd(char *func_name) {
    // 1. Verifica se a funcao esta prototipada na TSF
    type_symbol_function *func = sym_func_find(func_name);
    if (func == NULL) {
        printf("[ERRO] Funcao nao declarada ou nao prototipada: '%s'\n", func_name);
        return false;
    }

    match(OPEN_PAR);

    int nargs = 0;
    char arg_names[MAX_PARAMS][MAX_CHAR]; // guarda nomes para verificar apos o loop

    // Processa lista de argumentos
    if (lookahead->tag != CLOSE_PAR) {
        do {
            if (lookahead->tag != ID) {
                printf("[ERRO] Esperado identificador como argumento %d de '%s'.\n",
                       nargs + 1, func_name);
                return false;
            }

            char arg_name[MAX_CHAR];
            strcpy(arg_name, lookahead->lexema);

            // Verifica se o argumento esta declarado como variavel
            type_symbol_table_entry *var = sym_find(arg_name, &global_symbol_table_variables);
            if (var == NULL) {
                printf("[ERRO] Variavel '%s' nao declarada (argumento %d de '%s').\n",
                       arg_name, nargs + 1, func_name);
                return false;
            }

            // 3a. Verifica se o indice ainda esta dentro do esperado antes de checar
            if (nargs < func->nparams) {
                // 3b. Verifica o tipo do argumento
                if (var->type != func->params[nargs].type) {
                    printf("[ERRO] Tipo incompativel no argumento %d de '%s'. "
                           "Esperado tipo %d, recebido tipo %d.\n",
                           nargs + 1, func_name,
                           func->params[nargs].type, var->type);
                    return false;
                }
                // 3c. Verifica o nome (id) do argumento
                if (strcmp(var->name, func->params[nargs].name) != 0) {
                    printf("[ERRO] Nome do argumento %d incorreto em '%s'. "
                           "Esperado '%s', recebido '%s'.\n",
                           nargs + 1, func_name,
                           func->params[nargs].name, var->name);
                    return false;
                }
            }

            strcpy(arg_names[nargs], arg_name);
            match(ID);
            nargs++;

            if (lookahead->tag == COMMA) {
                match(COMMA);
            } else {
                break;
            }
        } while (true);
    }

    match(CLOSE_PAR);

    // 2. Verifica quantidade de argumentos
    if (nargs != func->nparams) {
        printf("[ERRO] Numero de argumentos incorreto em chamada de '%s'. "
               "Esperado: %d, Recebido: %d.\n",
               func_name, func->nparams, nargs);
        return false;
    }

    // Gera instrucao de salto para a funcao (jal label)
    gen_call(func->label);

    return match(SEMICOLON);
}

/**
 * @brief Regra de derivacao Func_code
 * Func_code -> func_implementation Func_code | <vazio>
 */
void func_code(void) {
    while (func_implementation());
}

/**
 * @brief Regra de derivacao para implementacao de uma funcao
 * func_implementation -> {tipo} id ( [{tipo} id {, {tipo} id}*] ) begin statements end
 * @return int true/false
 */
int func_implementation(void) {
    int type = lookahead->tag;
    if (type != INT && type != FLOAT && type != CHAR && type != STRING) {
        return false;
    }

    int return_type = type;
    match(type);

    char name[MAX_CHAR];
    strcpy(name, lookahead->lexema);
    match(ID);

    // Garante que somente funcoes prototipadas possam ser implementadas
    type_symbol_function *func = sym_func_find(name);
    if (func == NULL) {
        printf("[ERRO] Funcao '%s' nao foi prototipada. Implemente apenas funcoes prototipadas.\n", name);
        return false;
    }

    if (func->return_type != return_type) {
        printf("[ERRO] Tipo de retorno da implementacao nao corresponde ao prototipo de '%s'.\n", name);
        return false;
    }

    match(OPEN_PAR);

    type_param impl_params[MAX_PARAMS];
    int impl_nparams = 0;

    while (lookahead->tag != CLOSE_PAR) {
        int param_type = lookahead->tag;
        match(param_type);

        char param_name[MAX_CHAR];
        strcpy(param_name, lookahead->lexema);
        match(ID);

        impl_params[impl_nparams].type = param_type;
        strcpy(impl_params[impl_nparams].name, param_name);
        impl_nparams++;

        if (lookahead->tag == COMMA) {
            match(COMMA);
        }
    }

    match(CLOSE_PAR);

    // Verifica correspondencia de parametros com o prototipo
    if (impl_nparams != func->nparams) {
        printf("[ERRO] Numero de parametros na implementacao de '%s' nao corresponde ao prototipo.\n", name);
        return false;
    }

    for (int i = 0; i < impl_nparams; i++) {
        if (impl_params[i].type != func->params[i].type ||
            strcmp(impl_params[i].name, func->params[i].name) != 0) {
            printf("[ERRO] Parametro %d na implementacao de '%s' nao corresponde ao prototipo "
                   "(esperado '%s' tipo %d, recebido '%s' tipo %d).\n",
                   i + 1, name,
                   func->params[i].name, func->params[i].type,
                   impl_params[i].name, impl_params[i].type);
            return false;
        }
    }

    match(BEGIN);
    gen_label(func->label); // Emite o label da funcao no codigo de montagem
    statements();
    match(END);
    gen_func_epilog();      // Emite jr $ra (retorno da funcao)

    return true;
}

/**
 * @brief Regra de derivacao que analisa expressoes booleanas
 */
int B() {
    int operator;
    if (E()) {
        if (boolOperator(&operator)) {
            if (E()) {
                gen_bool(operator);
                return true;
            }
        }
    }
    return false;
}

int boolOperator(int *operator) {
    int lookahead_tag = lookahead->tag;
    *operator = -1;

    if (lookahead_tag == EQUAL ||
        lookahead_tag == NE    ||
        lookahead_tag == LT    ||
        lookahead_tag == GT    ||
        lookahead_tag == LE    ||
        lookahead_tag == GE) {
        *operator = lookahead_tag;
        match(lookahead_tag);
        return true;
    } else {
        printf("[ERRO] Operador relacional esperado. Encontrado: %s\n", lookahead->lexema);
        return false;
    }
}

/* ------- Gramatica de expressoes aritmeticas ------- */

int E() {
    int b1, b2;
    b1 = T();
    if (b1) b2 = ER();
    return b1 && b2;
}

int ER() {
    if (lookahead->tag == '+') {
        int b1, b2;
        match('+');
        b1 = T();
        genAdd();
        if (b1) b2 = ER();
        return b1 && b2;
    } else if (lookahead->tag == '-') {
        int b1, b2;
        match('-');
        b1 = T();
        genSub();
        if (b1) b2 = ER();
        return b1 && b2;
    } else if (lookahead->tag == ')'
            || lookahead->tag == SEMICOLON
            || lookahead->tag == COMMA
            || lookahead->tag == ENDTOKEN
            || lookahead->tag == GT
            || lookahead->tag == LT
            || lookahead->tag == GE
            || lookahead->tag == LE
            || lookahead->tag == EQUAL
            || lookahead->tag == NE) {
        return true;
    } else {
        return false;
    }
}

int T() {
    int b1, b2;
    b1 = F();
    if (b1) b2 = TR();
    return b1 && b2;
}

int TR() {
    if (lookahead->tag == '*') {
        int b1, b2;
        match('*');
        b1 = F();
        genMult();
        if (b1) b2 = TR();
        return b1 && b2;
    } else if (lookahead->tag == '/') {
        int b1, b2;
        match('/');
        b1 = F();
        genDiv();
        if (b1) b2 = TR();
        return b1 && b2;
    } else if (lookahead->tag == ')'
            || lookahead->tag == SEMICOLON
            || lookahead->tag == COMMA
            || lookahead->tag == ENDTOKEN
            || lookahead->tag == '+'
            || lookahead->tag == '-'
            || lookahead->tag == GT
            || lookahead->tag == LT
            || lookahead->tag == GE
            || lookahead->tag == LE
            || lookahead->tag == EQUAL
            || lookahead->tag == NE) {
        return true;
    } else {
        return false;
    }
}

int F() {
    if (lookahead->tag == '(') {
        int b1, b2;
        match('(');
        b1 = E();
        if (b1) b2 = match(')');
        return b1 && b2;
    } else if (lookahead->tag == NUM) {
        char lexema[MAX_TOKEN];
        strcpy(lexema, lookahead->lexema);
        int b1 = match(NUM);
        genNum(lexema);
        return b1;
    } else if (lookahead->tag == ID) {
        char lexema[MAX_TOKEN];
        strcpy(lexema, lookahead->lexema);
        if (sym_find(lexema, &global_symbol_table_variables) == NULL) {
            printf("[ERRO] Variavel nao declarada: %s\n", lexema);
            return false;
        }
        int b1 = match(ID);
        gen_id_value(lexema);
        return b1;
    } else {
        return false;
    }
}

/* ------- MAIN ------- */

int main(int argc, char *argv[]) {
    initSymbolTableVariables(&global_symbol_table_variables);
    initSymbolTableString();
    initSymbolTableFunctions();

    if (argc != 2) {
        printf("[ERRO] Informe o arquivo de entrada como parametro.\n\n");
        exit(EXIT_FAILURE);
    }

    initLex(argv[1]);
    lookahead = getToken();

    strcpy(output_file_name, argv[1]);
    strcat(output_file_name, ".asm");
    output_file = fopen(output_file_name, "w+");

    program();

    fclose(output_file);
    return 1;
}