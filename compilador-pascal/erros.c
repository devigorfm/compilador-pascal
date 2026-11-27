#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erros.h"

static Erro *erros = NULL;   /* o vetor de erros */
static int total = 0;        /* quantos erros ja foram guardados */
static int capacidade = 0;   /* capacidade do vetor */

void registrarErro(const char *fase, int linha, int coluna,
                   const char *lexema, const char *descricao) {
    /* Se o vetor encher, dobra o tamanho dele */
    if (total == capacidade) {
        capacidade = (capacidade == 0) ? 16 : capacidade * 2;
        Erro *novo = realloc(erros, capacidade * sizeof(Erro));
        if (novo == NULL) {
            fprintf(stderr, "Memoria insuficiente\n");
            exit(1);
        }
        erros = novo;
    }

    /* Preenche a proxima posicao livre.
       snprintf copia o texto sem passar do tamanho do campo. */
    Erro *e = &erros[total];
    snprintf(e->fase, sizeof(e->fase), "%s", fase);
    e->linha = linha;
    e->coluna = coluna;
    snprintf(e->lexema, sizeof(e->lexema), "%s", lexema);
    snprintf(e->descricao, sizeof(e->descricao), "%s", descricao);
    total++;
}

int totalErros(void) {
    return total;
}

void imprimirTabelaErros(void) {
    printf("\nTABELA DE ERROS\n");
    if (total == 0) {
        printf("Nenhum erro encontrado.\n");
        return;
    }
    printf("%-10s %-6s %-6s %-16s %s\n", "FASE", "LINHA", "COLUNA", "LEXEMA", "DESCRICAO");
    for (int i = 0; i < total; i++) {
        printf("%-10s %-6d %-6d %-16s %s\n", erros[i].fase, erros[i].linha,
               erros[i].coluna, erros[i].lexema, erros[i].descricao);
    }
}

void liberarErros(void) {
    free(erros);
    erros = NULL;
    total = 0;
    capacidade = 0;
}
