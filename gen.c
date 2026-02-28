/**
 * @file gen.c
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Codificacao do modulo gerador de codigo
 * @version 0.2
 * @date 2022-02-23
 * 
 */

// Inclusao do cabecalho
#include "gen.h"

// Inclusao explicita de variaveis globais de outro contexto (symbols.h)
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
char output_file_name[MAX_CHAR];
FILE *output_file;

/**
 * @brief Funcao que gera codigo de montagem para SOMA
 * 
 */
void genAdd() {
    fprintf(output_file, ";Adicao\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "pop rbx\n");
    fprintf(output_file, "add rax,rbx\n");
    fprintf(output_file, "push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para SUBTRACAO
 * 
 */
void genSub() {
    fprintf(output_file, ";Subtracao\n");
    fprintf(output_file, "pop rbx\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "sub rax,rbx\n");
    fprintf(output_file, "push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para MULTIPLICACAO
 * 
 */
void genMult() {
    fprintf(output_file, ";Multiplicacao\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "pop rbx\n");
    fprintf(output_file, "imult rax,rbx\n");
    fprintf(output_file, "push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para DIVISAO
 * 
 */
void genDiv() {
    fprintf(output_file, ";Divisao\n");
    fprintf(output_file, "pop rbx\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "idiv rax,rbx\n");
    fprintf(output_file, "push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para armazenamento de NUMERAL
 * 
 * @param num_string 
 */
void genNum(char num_string[MAX_TOKEN]) {
    fprintf(output_file, ";Amarzenamento de numero\n");
    fprintf(output_file, "mov rax,%s\n", num_string);
    fprintf(output_file, "push rax\n");
}

/**
 * @brief gera codigo de atribuicao de valor a uma variavel
 * exemplo: mov [id], eax
 * @param char* nome da variavel
 */
void gen_assign(char *var_name){
    fprintf(output_file, ";Atribuicao\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "mov [%s], eax\n", var_name);
}

/**
 * @brief Funcao que gera um preambulo que permite o uso das funcoes do C (scanf e printf)
 * 
 */
void gen_preambule(void) {
    fprintf(output_file, ";UFMT-Compiladores\n");
    fprintf(output_file, ";Prof. Ivairton\n");
    fprintf(output_file, ";Procedimento para geracao do executavel apos compilacao (em Linux):\n");
    fprintf(output_file, ";(1) compilacao do Assembly com nasm: $ nasm -f elf64 <nome_do_arquivo>\n");
    fprintf(output_file, ";(2) likedicao: $ ld -m elf_x86_64 <nome_arquivo_objeto>\n\n");
    fprintf(output_file, "extern printf\n");
    fprintf(output_file, "extern scanf\n");
}

/**
 * @brief Funcao que gera codigo da secao de dados (declaracao de variaveis).
 */
void gen_data_section(void) {
    int i, n;
    
    fprintf(output_file, "\n\tsection .data\n");
    fprintf(output_file, "buffer_io db 0, 0\n"); // Buffer para leitura de input do usuario

    // emite strings de formato fixo
    //fprintf(output_file, "str0: db \"%%d\",13,10,0\n");
    //fprintf(output_file, "str1: db \"%%s\",13,10,0\n");
    //fprintf(output_file, "str2: db \"%%c\",13,10,0\n");
    //fprintf(output_file, "str3: db \"%%lf\",13,10,0\n");
    //fprintf(output_file, "\n");

    // processa cada simbolo da tabela de strings
    //n = symbol_table_string.n_strings;
    //for (i = 0; i < n; i++) {
    //    fprintf(output_file, "%s: db %s, 0\n", 
    //        symbol_table_string.string[i].name,
    //        symbol_table_string.string[i].value);
    //}
    //fprintf(output_file,"\n");
    
    // processa cada simbolo da tabela e gera um ponteiro para cada variavel na memoria
    n = global_symbol_table_variables.n_variables;
    for (i = 0; i < n; i++) {
       fprintf(output_file, "%s: ", global_symbol_table_variables.variable[i].name); 
       
       switch(global_symbol_table_variables.variable[i].type) { //Por enquanto gera endereco zero
            case INT:
                fprintf(output_file, "dd 0\n");
                break;
            case STRING:
                fprintf(output_file, "db \"                \" \n");
                break;
            case FLOAT:
                fprintf(output_file, "dd \"%%lf\", 16\n");
                break;
            case CHAR:
                fprintf(output_file, "dd \"%%c\", 1\n");
                break;
            default:
                fprintf(output_file, "[ERRO] Tipo desconhecido.\n");       
                break;           
       }
    }

    n = symbol_table_string.n_strings;
    for (i = 0; i < n; i++) {
        fprintf(output_file, "%s: db %s\n",
            symbol_table_string.string[i].name, 
            symbol_table_string.string[i].value);
    }
}

/**
 * @brief Funcao que gera a marcacao do inicio da secao de codigo
 * 
 */
void gen_preambule_code(void) {
    fprintf(output_file, "\n\tsection .text\n");
    fprintf(output_file, "\tglobal main,_start\n");
    fprintf(output_file, "main:\n");
    fprintf(output_file, "_start:\n");
}

/**
 * @brief Funcao que encerra o codigo inserindo comandos de fechamento
 * 
 */
void gen_epilog_code(void) {
    //fprintf(output_file, "\nret\n");
    fprintf(output_file, "\n;encerra programa\n");
    fprintf(output_file, "mov ebx,0\n");
    fprintf(output_file, "mov eax,1\n");
    fprintf(output_file, "int 0x80\n");
}

/**
 * @brief Funcao que gera automaticamente um novo nome para um label
 * @param string name
 */
void gen_label_name( char *name ) {
    char str_name[MAX_CHAR];
    char conv_value[16];
    static int nlabels=0;

    sprintf(conv_value, "%d", nlabels);
    strcpy(str_name, "label");
    strcat(str_name, conv_value);
    strcpy(name, str_name);
    nlabels++;
}

/**
 * @brief Funcao que registra no codigo um label
 * @param string label 
 */
void gen_label( char *label ) {
    fprintf(output_file, "%s:\n", label);
}

/**
 * @brief Funcao que gera um jump condicional
 * @param string label
 */
void gen_cond_jump(char *label) {
    fprintf(output_file, ";jump condicional\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "cmp rax, 0\n");
    fprintf(output_file, "jz %s\n", label);
}

/**
 * @brief Funcao que gera um jump incondicional
 * @param string label
 */
void gen_incond_jump(char *label) {
    fprintf(output_file, ";jump incondicional\n");
    fprintf(output_file, "jmp %s\n", label);
}

/**
 * @brief Funcao que gera codigo a partir do processamento da expressao logica
 * 
 * @param oper 
 */
void gen_bool(int oper) {
    static int bool_label;
    char bool_label_name[MAX_CHAR];
    fprintf(output_file,";Aplica operador booleano/exp.logica\n");
    fprintf(output_file,"pop rbx\n");
    fprintf(output_file,"pop rax\n");
    gen_bool_label_name(bool_label_name);
    fprintf(output_file,"mov rcx,1\n");
    fprintf(output_file,"cmp eax,ebx\n");
    switch (oper) {
        case EQUAL:
            fprintf(output_file,"je %s\n", bool_label_name);
            break;
        case NE:
            fprintf(output_file,"jne %s\n",bool_label_name);
            break;
        case LT:
            fprintf(output_file,"jl %s\n", bool_label_name);
            break;
        case GT:
            fprintf(output_file,"jg %s\n", bool_label_name);
            break;
        case LE:
            fprintf(output_file,"jle %s\n", bool_label_name);
            break;
        case GE:
            fprintf(output_file,"jge %s\n", bool_label_name);
            break;
        default:
            printf("[ERRO] operador booleano nao suportado.\n");
            break;
    }
    fprintf(output_file, "mov rcx,0\n");
    gen_label(bool_label_name);
    fprintf(output_file, "mov rax, rcx\n");
    fprintf(output_file, "push rax\n");    
}

void gen_bool_label_name(char *name) {
    static int nbool_labels = 0;
    char conv_value[16];
    sprintf(conv_value, "%d", nbool_labels);
    strcpy(name, "bool_label");
    strcat(name, conv_value);
    nbool_labels++;
}

/**
 * @brief Gera codigo para o comando READ
 * 
 * @param lexeme_of_id nome do identificador
 */
void gen_read(char *lexeme_of_id, int type) {
    /*
    //Codigo antigo, demanda estudo para ser usado com o liker GCC no Linux
    fprintf(output_file, "mov rdi, fmtstr0\n");
    fprintf(output_file, "mov rsi, %s\n", lexeme_of_id);
    fprintf(output_file, "mov rax, 0\n");
    fprintf(output_file, "call scanf\n");
    */
    switch (type) {
        case INT:
            fprintf(output_file, "\n; --- le valor inteiro ---\n");
            // 1. Lê do teclado para o buffer
            fprintf(output_file, "mov edx, 2\n"); // Lê 1 dígito + a tecla ENTER
            fprintf(output_file, "mov ecx, buffer_io\n"); // Guarda no rascunho
            fprintf(output_file, "mov ebx, 0\n"); // 0 = Teclado
            fprintf(output_file, "mov eax, 3\n"); // sys_read
            fprintf(output_file, "int 0x80\n");

            // 2. Converte ASCII para Número Real
            fprintf(output_file, "movzx eax, byte [buffer_io]\n"); // Pega o caractere digitado
            fprintf(output_file, "sub eax, 48\n"); // Transforma '5' (53) em 5
            fprintf(output_file, "mov dword [%s], eax\n", lexeme_of_id); // Salva na variável real
            break;
        case FLOAT:
            fprintf(output_file, "\n;le valor float\n");
            fprintf(output_file, "mov edx,16\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,3\n");
            fprintf(output_file, "int 0x80\n");
            break;
        case CHAR:
            fprintf(output_file, "\n;le valor char\n");
            fprintf(output_file, "mov edx,1\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,3\n");
            fprintf(output_file, "int 0x80\n");
            break;
        case STRING:
            fprintf(output_file, "\n;le valor string\n");
            fprintf(output_file, "mov edx,16\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,3\n");
            fprintf(output_file, "int 0x80\n");
            break;
    }
}

/**
 * @brief Gera codigo para o comando WRITE
 * 
 * @param lexeme_of_id nome do identificador
 */
void gen_write(char *lexeme_of_id, int type) {
    /*
    //Codigo antigo, demanda estudo para ser usado com o liker GCC no Linux
    fprintf(output_file, "mov rdi, fmtstr0\n");
    fprintf(output_file, "mov rsi, [rel %s]\n", lexeme_of_id);
    fprintf(output_file, "mov rax, 0\n");
    fprintf(output_file, "call printf\n");
    */
    switch (type) {
        case INT:
            fprintf(output_file, "\n; --- escreve valor inteiro ---\n");
            // 1. Converte Número Real para ASCII
            fprintf(output_file, "mov eax, dword [%s]\n", lexeme_of_id); // Pega o número matemático
            fprintf(output_file, "add eax, 48\n"); // Transforma 5 em '5' (53)
            fprintf(output_file, "mov byte [buffer_io], al\n"); // Salva o texto no rascunho

            // 2. Imprime na tela
            fprintf(output_file, "mov edx, 1\n"); // Imprime apenas 1 caractere
            fprintf(output_file, "mov ecx, buffer_io\n"); // Imprime o que está no rascunho
            fprintf(output_file, "mov ebx, 1\n"); // 1 = Tela
            fprintf(output_file, "mov eax, 4\n"); // sys_write
            fprintf(output_file, "int 0x80\n");
            break;
        case FLOAT:
            fprintf(output_file, "\n;escreve valor float\n");
            fprintf(output_file, "mov edx,16\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,4\n");
            fprintf(output_file, "int 0x80\n");
            break;
        case CHAR:
            fprintf(output_file, "\n;escreve valor char\n");
            fprintf(output_file, "mov edx,1\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,4\n");
            fprintf(output_file, "int 0x80\n");
            break;
        case STRING:
            fprintf(output_file, "\n;escreve valor string\n");
            fprintf(output_file, "mov edx,16\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,4\n");
            fprintf(output_file, "int 0x80\n");
            break;
    }
}

void gen_call(char *label){
    fprintf(output_file, ";Chamada de funcao\n");
    fprintf(output_file, "jal %s\n", label);
}

/**
 * @brief Funcao que gera codigo de retorno de funcao (ret)
 * 
 */

void gen_func_epilog(void) {
    fprintf(output_file, "\n;Retorna da funcao\n");
    fprintf(output_file, "jr $ra\n");
}

/**
 * @brief Regra de derivação para implementação de funções
 */
void gen_id_value(char *id_name) {
    fprintf(output_file, ";Carrega valor de variavel\n");
    fprintf(output_file, "mov eax, dword [%s]\n", id_name);
    fprintf(output_file, "push rax\n");
}