#ifndef ERROS_H
#define ERROS_H

/* Uma linha da tabela de erros */
typedef struct {
    char fase[12];         /* "LEXICO", "SINTATICO" ou "SEMANTICO" */
    int linha;             /* linha onde o erro aconteceu */
    int coluna;            /* coluna onde o erro aconteceu */
    char lexema[100];      /* o texto que causou o erro */
    char descricao[200];   /* explicacao do erro */
} Erro;

/* Guarda um erro novo na tabela */
void registrarErro(const char *fase, int linha, int coluna,
                   const char *lexema, const char *descricao);

/* Quantos erros existem no total */
int totalErros(void);

/* Imprime a tabela de erros */
void imprimirTabelaErros(void);

/* Libera a memoria usada pela tabela */
void liberarErros(void);

#endif
