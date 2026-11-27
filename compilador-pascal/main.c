#include <stdio.h>
#include "lexer.h"
#include "erros.h"

int main(int argc, char *argv[]) {
    /* argv[0] e o nome do programa e argv[1] e o arquivo .pas */
    if (argc != 2) {
        printf("Uso: %s arquivo.pas\n", argv[0]);
        return 1;
    }

    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro: nao foi possivel abrir o arquivo '%s'\n", argv[1]);
        return 1;
    }

    /* ===== ANALISE LEXICA ===== */
    printf("========== ANALISE LEXICA ==========\n");
    ListaTokens tokens;
    int errosLexicos = analisarLexico(arquivo, &tokens);
    fclose(arquivo);
    imprimirTabelaTokens(&tokens);

    if (errosLexicos > 0) {
        printf("\nForam encontrados %d erro(s) lexico(s).\n", errosLexicos);
    } else {
        printf("\nAnalise lexica concluida sem erros.\n");
    }

    /* ===== RESULTADO ===== */
    printf("\n========== RESULTADO ==========\n");
    imprimirTabelaErros();

    int total = totalErros();

    /* Libera toda a memoria usada */
    liberarTokens(&tokens);
    liberarErros();

    return (total == 0) ? 0 : 1;
}
