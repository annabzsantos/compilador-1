#include "symbols.h"

extern int nstringconsts;
int sympos = 0;
int nstringconsts = 0;
int symfuncspos = 0; //Sera usado posteriormente
int stack_pos = 0;

//Variaveis globais
type_symbol_table_variables global_symbol_table_variables;
type_symbol_table_string symbol_table_string;
type_symbol_function symfuncs[MAX_FUNCS];
 

/**
 * @brief Busca pelo simbolo na tabela de variaveis
 * 
 * @param char*
 * @param type_symbol_table_variables*
 * @return type_symbol_table_entry* 
 */
type_symbol_table_entry *sym_find(char *s, type_symbol_table_variables *stv) {
	int i;
	type_symbol_table_entry *symbol = NULL;
	for (i = 0; i < stv->n_variables; i++) {
		if (strcmp(stv->variable[i].name, s) == 0) {
			symbol = &(stv->variable[i]);
		}
	}
	return symbol;
}

/**
 * @brief Insere um novo simbolo na tabela de variaveis
 * 
 * @param name 
 * @param type 
 * @param addr 
 * @return type_symbol* 
 */
 type_symbol_table_entry *sym_declare(char *name, int type, int addr, type_symbol_table_variables *stv) {
	if (stv->n_variables < MAX_SYMBOLS) { //Verifica se eh possivel declarar mais variavel
		int n_var;
        n_var = stv->n_variables;

        stv->variable[n_var].type = type;
        stv->variable[n_var].addr = addr;
        strcpy(stv->variable[n_var].name, name);

        stv->n_variables++;

        return &(stv->variable[n_var]);   // CORRIGIDO
	} else {
        printf("[ERRO] Limite de declaracao (quantidade) de variaveis atingido.\n");
        return NULL;
    }
}

/**
 * @brief Busca pelo simbolo na tabela de STRINGS
 * 
 * @param char*
 * @return type_symbol_table_string_entry* 
 */
type_symbol_table_string_entry *sym_string_find(char *s) {
    int i;
    type_symbol_table_string_entry *str = NULL;
    for (i = 0; i < symbol_table_string.n_strings; i++ ) {
        if ( strcmp(symbol_table_string.string[i].value, s) == 0 ) {
            str = &(symbol_table_string.string[i]);
        }
    }
    return str;
}

/**
 * @brief Insere um novo simbolo na tabela de STRINGS
 * 
 * @param char*
 * @return type_symbol_table_string_entry* 
 */
type_symbol_table_string_entry *sym_string_declare(char *s) {
    static int str_label_count = 0;

    if ( symbol_table_string.n_strings < MAX_SYMBOLS ) {
        int n_str;
        char str_name[MAX_TOKSZ];

        sprintf(str_name, "str%d", str_label_count);

        n_str = symbol_table_string.n_strings;

        strcpy(symbol_table_string.string[n_str].value, s);
        strcpy(symbol_table_string.string[n_str].name, str_name);

        str_label_count++;
        symbol_table_string.n_strings++;

        return &(symbol_table_string.string[n_str]) ;
    } else {
        printf("[ERRO] Numero maximo de strings declaradas alcancado!\n");
        return NULL;
    }
}

/**
 * @brief Busca pelo simbolo na tabela (funcoes)
 * 
 * @param s 
 * @return type_symbol_function* 
 */
type_symbol_function *sym_func_find(char *s) {
    int i;
    for (i = 0; i < symfuncspos; i++) {
        if (strcmp(symfuncs[i].name, s) == 0) {
            return &symfuncs[i];
        }
    }
    return NULL;
}


/**
 * @brief Insere um novo simbolo na tabela (funcoes)
 * 
 * @param name 
 * @param type 
 * @param params 
 * @param nparams 
 * @return type_symbol_function* 
 */
type_symbol_function *sym_func_declare(char *name, int return_type, type_param *params, int nparams) {

    if (symfuncspos >= MAX_FUNCS) {
        printf("[ERRO] Limite de funcoes atingido.\n");
        return NULL;
    }

    type_symbol_function *f = &symfuncs[symfuncspos];

    strcpy(f->name, name);
    f->return_type = return_type;
    f->nparams = nparams;

    int i;
    for (i = 0; i < nparams; i++) {
        strcpy(f->params[i].name, params[i].name);
        f->params[i].type = params[i].type;
    }

    // Gera label
    sprintf(f->label, "func_%s", name);

    symfuncspos++;

    return f;
}

void initSymbolTableFunctions() {
    symfuncspos = 0;
}


/**
 * @brief Inicializa tabela de simbolos de variaveis globais
 * 
 * @param stv 
 */
void initSymbolTableVariables(type_symbol_table_variables *stv) {
    int i;
    stv->n_variables = 0;
    for (i = 0; i < MAX_SYMBOLS; i++) {
        strcpy(stv->variable[i].name, "");
        stv->variable[i].type = -1;
        stv->variable[i].addr = 0;
    }
    stv->next = NULL;
}

/**
 * @brief Inicializa tabela de simbolos de strings
 * 
 */
void initSymbolTableString() {
    int i;
    symbol_table_string.n_strings = 0;
    for (i = 0; i < MAX_SYMBOLS; i++) {
        strcpy(symbol_table_string.string[i].name, "");
        strcpy(symbol_table_string.string[i].value, "");
    }
}

///Para DEBUG
void printSTVariables(type_symbol_table_variables *stv) {
    int i, n_var;
    printf("TABELA DE SIMBOLOS DE VARIAVEIS:\n");
    n_var = stv->n_variables;
    printf("Numero de variaveis = %d\n", n_var);
    for (i = 0; i < n_var; i++) {
        printf("\tvariavel[%d].name:%s\n", i, stv->variable[i].name);
        printf("\tvariavel[%d].type:%d\n", i, stv->variable[i].type);
    }
}

void printSTString() {
    int i, n_str;
    printf("TABELA DE SIMBOLOS DE STRINGS:\n");
    n_str = symbol_table_string.n_strings;
    printf("Numero de strings = %d\n", n_str);
    for (i = 0; i < n_str; i++) {
        printf("\tstring[%d].name:%s\n", i, symbol_table_string.string[i].name);
        printf("\tstring[%d].value:%s\n", i, symbol_table_string.string[i].value);
    }
}

void printSTFunctions() {
    int i, j;
    printf("TABELA DE SIMBOLOS DE FUNCOES:\n");
    printf("Numero de funcoes = %d\n", symfuncspos);
    for (i = 0; i < symfuncspos; i++) {
        printf("\tfuncao[%d].name: %s\n", i, symfuncs[i].name);
        printf("\tfuncao[%d].return_type: %d\n", i, symfuncs[i].return_type);
        printf("\tfuncao[%d].label: %s\n", i, symfuncs[i].label);
        printf("\tfuncao[%d].nparams: %d\n", i, symfuncs[i].nparams);
        for (j = 0; j < symfuncs[i].nparams; j++) {
            printf("\t\tparam[%d]: name %s, type %d\n", j, symfuncs[i].params[j].name, symfuncs[i].params[j].type);
        }
    }
}