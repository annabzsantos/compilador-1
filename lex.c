/**
 * @file lex.c
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Codificacao do modulo do analisador lexico
 * @version 0.4
 * @date 2022-03-16
 * 
 */

// Inclusao do cabecalho
#include "lex.h"

// Variaveis globais
int pos;
FILE *input_file;
//Definicao e inicializacao de estrutura
type_token key_words[] = {
    {IF, "if", 0},
    {THEN, "then", 0},
    {ELSE, "else", 0},
    {WHILE, "while", 0},
    {DO, "do", 0},
    {NUM, "integer", 0},
    {READ, "read", 0},
    {WRITE, "write", 0},
    {INT, "int", 0},
    {FLOAT, "float", 0},
    {STRING, "string", 0},
    {CHAR, "char", 0},
    {BEGIN, "begin", 0},
    {END, "end", 0},
    {EQUAL, "==",0},
    {NE, "!=",0},
    {GE, ">=",0},
    {LE, "<=",0}
};


/**
 * @brief Processo de inicializacao do lexico. Recebe o arquivo de entrada
 * contendo codigo e carrega-o em memoria.
 * 
 */
void initLex(char input_file_name[]) {
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        printf("[ERRO]\n\tArquivo de entrada não encontrado: %s\n\n", input_file_name);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Analisa o proximo comando no código de entrada e retorna estrutura de
 * dados com Token correspondente (ponteiro).
 * 
 * @return type_token* 
 */
type_token *getToken() {
    char buffer[MAX_CHAR];
    int pos_buffer;
    type_token *token;
    type_token *key_found;
    char ch;
    
    pos_buffer = 0;
    token = (type_token*) malloc(sizeof(type_token));
    strcpy(buffer, "");
    ch = fgetc(input_file);

    // Consome espacos, tabulacoes e quebras de linha
    while ( ch == ' ' || ch == '\t' || ch == '\n') {
        ch = fgetc(input_file);
    }

    // Verifica se NUMERO
    if ( isdigit(ch) ) {
        // constroi buffer com os digitos
        while ( isdigit(ch) ) {
            buffer[pos_buffer++] = ch;
            ch = fgetc(input_file);
        }
        //Verifica se eh ponto flutuante
        if ( ch == '.' ) {
            buffer[pos_buffer++] = ch;
            ch = fgetc(input_file);
            while ( isdigit(ch) ) {
                buffer[pos_buffer++] = ch;
                ch = fgetc(input_file);
            }
        }
        ungetc(ch, input_file);
        buffer[pos_buffer] = '\0';
        token->tag = NUM;
        strcpy( token->lexema, buffer ); //copia buffer para lexema
    } //Verifica se entrada eh um alfa-numerico (palavra reservada ou identificador)
    else if ( isalpha(ch) ) {
        buffer[pos_buffer++] = ch;
        ch = fgetc(input_file);
        while( isalnum(ch) ) {
            buffer[pos_buffer++] = ch;
            ch = fgetc(input_file);
        }
        ungetc(ch, input_file);
        buffer[pos_buffer] = '\0';
        key_found = keyWordFind(buffer);
        if (key_found != NULL) { //Palavra reservada
            token->tag = key_found->tag;
            strcpy(token->lexema, key_found->lexema);
            token->value = key_found->value;
            //o token sera retornado no final da funcao
        } else { //Identificador
            token->tag = ID;
            strcpy(token->lexema, buffer);
            token->value = 0;
        }
    }
    //Verifica se PLUS (+)
    else if (ch == PLUS) {
        token->tag = PLUS;
        strcpy(token->lexema, "+");
        token->value = 0;
    }
    //Verifica se MINUS (-)
    else if (ch == MINUS) {
        token->tag = MINUS;
        strcpy(token->lexema, "-");
        token->value = 0;
    }
    //Verifica se MULT (*)
    else if (ch == MULT) {
        token->tag = MULT;
        strcpy(token->lexema, "*");
        token->value = 0;
    }
    //Verifica se DIV (/)
    else if (ch == DIV) {
        token->tag = DIV;
        strcpy(token->lexema, "/");
        token->value = 0;
    }
    //Verifica se OPEN_PAR -> "("
    else if (ch == OPEN_PAR) {
        token->tag = OPEN_PAR;
        strcpy(token->lexema, "(");
        token->value = 0;
    }
    //Verifica se CLOSE_PAR -> ")"
    else if (ch == CLOSE_PAR) {
        token->tag = CLOSE_PAR;
        strcpy(token->lexema, ")");
        token->value = 0;
    }
    //Verifica se SEMICOLON - > ";"
    else if (ch == SEMICOLON) {
        token->tag = SEMICOLON;
        strcpy(token->lexema, ";");
        token->value = 0;
    }
    //Verifica se COMMA -> ","
    else if (ch == COMMA) {
        token->tag = COMMA;
        strcpy(token->lexema, ",");
        token->value = 0;
    }
    //Verifica se EQUAL ou ASSIGN -> '==' ou '=' 
    else if (ch == ASSIGN) {
        //Primeiro trata o caso de '=='. O caso '=' é feito por exclusão.
        ch = fgetc(input_file);
        if (ch == ASSIGN) {
            token->tag = EQUAL;
            strcpy(token->lexema, "==");
            token->value = 0; 
        } else {
            ungetc(ch,input_file);  
            token->tag = ASSIGN;
            strcpy(token->lexema, "=");
            token ->value = 0; 
        }     
    }
    //Verifica se LT ou LE -> '<' ou '<='
    else if (ch == LT) {
        //Primeiro trata '<='
        ch = fgetc(input_file);
        if (ch == ASSIGN) {
            token->tag = LE;
            strcpy(token->lexema, "<=");
            token->value = 0;
        } else {
            ungetc(ch, input_file);
            token->tag = LT;
            strcpy(token->lexema, "<");
            token->value = 0;
        }
    }

    //TODO: implemente para GT e GE
    else if (ch == GT) {
        //Primeiro trata '>='
        ch = fgetc(input_file);
        if (ch == ASSIGN) {
            token->tag = GE;
            strcpy(token->lexema, ">=");
            token->value = 0;
        } else {
            ungetc(ch, input_file);
            token->tag = GT;
            strcpy(token->lexema, ">");
            token->value = 0;
        }
    }

    // Verifica aspas duplas
    else if (ch == DOUBLE_QUOTES) {
        buffer[pos_buffer++] = ch; // Guarda a aspa inicial
        ch = fgetc(input_file);
        
        // Consome ate encontrar o fechamento ou estourar o limite da memoria
        while ( (ch != DOUBLE_QUOTES) && (ch != EOF) && (pos_buffer < MAX_CHAR - 2) ) {
            buffer[pos_buffer++] = ch;
            ch = fgetc(input_file);
        }
        
        // Guarda a aspa final
        if (ch == DOUBLE_QUOTES) {
            buffer[pos_buffer++] = ch; 
        }
        
        buffer[pos_buffer] = '\0'; // Fecha a string no padrao C
        
        token->tag = STRING;
        strcpy(token->lexema, buffer);
        token->value = 0;
    }
    //Verifica se FIM DE ARQUIVO (EOF)
    else if (ch == EOF) {
        token->tag = ENDTOKEN;
        strcpy(token->lexema, "\0");
    }
    //ERRO
    else {
        token->tag = ERROR;
        strcpy(token->lexema, "");
    }

    return token;
}

/**
 * @brief Busca na lista de palavras reservadas o parâmetro informado, retornando
 * o respectivo token, ou nulo caso nao exista.
 * 
 * @param char* palavra a ser verificada se corresponde a uma palavra reservada.
 * @return type_token* com o endereco de memoria do token correspondente, ou nulo.
 */
type_token *keyWordFind(char *word) {
    int i;
    i = 0;

    while( key_words[i].lexema[0] != '\0' ) {
        if ( strcmp(word, key_words[i].lexema) == 0 )
            return &key_words[i];
        i++;
    }
    return NULL;
}