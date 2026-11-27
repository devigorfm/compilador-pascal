#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

/* As categorias de token (o "tipo" de cada palavra) */
typedef enum {
    TK_PR,         /* palavra reservada: program, begin, if... */
    TK_ID,         /* identificador: nomes de variaveis e funcoes */
    TK_NUM_INT,    /* literal numerico inteiro: 10 */
    TK_NUM_REAL,   /* literal numerico real: 3.14 */
    TK_TEXTO,      /* literal de texto: 'ola' */
    TK_OP,         /* operador: + - * / := = <> < <= > >= */
    TK_DELIM,      /* delimitador: ; : , . ( ) */
    TK_EOF         /* fim do arquivo (colocado no final da lista) */
} TipoToken;

/* Um token: o texto (lexema), a categoria e onde ele aparece */
typedef struct {
    char lexema[100];
    TipoToken tipo;
    int linha;
    int coluna;
} Token;

/* Lista dinamica de tokens (cresce conforme precisa) */
typedef struct {
    Token *itens;
    int total;
    int capacidade;
} ListaTokens;

/* Faz a analise lexica do arquivo e preenche a lista.
   Devolve quantos erros lexicos foram encontrados. */
int analisarLexico(FILE *arquivo, ListaTokens *lista);

/* Imprime a tabela de simbolos inicial (lexema, token, linha, coluna) */
void imprimirTabelaTokens(const ListaTokens *lista);

/* Devolve o nome da categoria, para imprimir. Ex.: TK_ID -> "ID" */
const char *nomeTipoToken(TipoToken tipo);

/* Libera a memoria da lista */
void liberarTokens(ListaTokens *lista);

#endif
