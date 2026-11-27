#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "erros.h"

/* ---------- Palavras reservadas do nosso subconjunto de Pascal ---------- */

static const char *palavrasReservadas[] = {
    "program", "var", "begin", "end", "integer", "real", "boolean",
    "if", "then", "else", "while", "do", "writeln", "true", "false",
    "and", "or", "not", "div", "mod", "function", "procedure"
};

/* Quantidade de palavras */
static const int TOTAL_RESERVADAS =
    sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]);

static int ehPalavraReservada(const char *palavra) {
    for (int i = 0; i < TOTAL_RESERVADAS; i++) {
        if (strcmp(palavra, palavrasReservadas[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* ---------- O "cursor" que anda pelo texto ---------- */

static char *fonte;     /* o arquivo inteiro, como um texto */
static int pos;         /* posicao atual dentro do texto */
static int linha;       /* linha atual (comeca em 1) */
static int coluna;      /* coluna atual (comeca em 1) */

/* Caractere atual */
static char atual(void) {
    return fonte[pos];
}

/* Caractere seguinte ao atual. */
static char proximo(void) {
    if (fonte[pos] == '\0') return '\0';
    return fonte[pos + 1];
}

/* Anda uma posicao, atualizando linha e coluna */
static void avancar(void) {
    if (fonte[pos] == '\0') return;      /* chegou ao fim */
    if (fonte[pos] == '\n') {            /* quebra de linha: */
        linha++;                         /*   proxima linha  */
        coluna = 1;                      /*   volta para a coluna 1 */
    } else {
        coluna++;
    }
    pos++;
}

/* ---------- Leitura do arquivo para a memoria ---------- */

/* Le o arquivo inteiro com fgetc e devolve um texto terminado em '\0'.
   O vetor comeca com 1024 posicoes e dobra sempre que enche. */
static char *lerArquivo(FILE *arquivo) {
    int capacidadeTexto = 1024;
    int tamanho = 0;
    char *texto = malloc(capacidadeTexto);
    int c;

    if (texto == NULL) {
        fprintf(stderr, "Memoria insuficiente\n");
        exit(1);
    }

    while ((c = fgetc(arquivo)) != EOF) {
        if (tamanho + 1 >= capacidadeTexto) {   /* +1 para caber o '\0' */
            capacidadeTexto *= 2;
            char *novo = realloc(texto, capacidadeTexto);
            if (novo == NULL) {
                fprintf(stderr, "Memoria insuficiente\n");
                exit(1);
            }
            texto = novo;
        }
        texto[tamanho++] = (char)c;
    }
    texto[tamanho] = '\0';
    return texto;
}

/* ---------- Lista de tokens ---------- */

static void adicionarToken(ListaTokens *lista, const char *lexema,
                           TipoToken tipo, int linhaToken, int colunaToken) {
    if (lista->total == lista->capacidade) {
        lista->capacidade = (lista->capacidade == 0) ? 64 : lista->capacidade * 2;
        Token *novo = realloc(lista->itens, lista->capacidade * sizeof(Token));
        if (novo == NULL) {
            fprintf(stderr, "Memoria insuficiente\n");
            exit(1);
        }
        lista->itens = novo;
    }
    Token *t = &lista->itens[lista->total];
    snprintf(t->lexema, sizeof(t->lexema), "%s", lexema);
    t->tipo = tipo;
    t->linha = linhaToken;
    t->coluna = colunaToken;
    lista->total++;
}

/* ---------- O analisador lexico ---------- */

int analisarLexico(FILE *arquivo, ListaTokens *lista) {
    char lexema[100];   /* onde montamos o texto do token atual */
    int i;              /* posicao dentro de lexema */
    int erros = 0;      /* quantos erros lexicos encontramos */

    lista->itens = NULL;
    lista->total = 0;
    lista->capacidade = 0;

    fonte = lerArquivo(arquivo);
    pos = 0;
    linha = 1;
    coluna = 1;

    while (atual() != '\0') {
        char c = atual();

        /* Guardamos onde o token COMECA, para registrar linha e coluna certas */
        int linhaInicio = linha;
        int colunaInicio = coluna;

        /* ---- Espacos e quebras de linha: so pula ---- */
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            avancar();
        }

        /* ---- Comentario com chaves: { ... } ---- */
        else if (c == '{') {
            avancar();                                   /* pula o '{' */
            while (atual() != '}' && atual() != '\0') {
                avancar();
            }
            if (atual() == '\0') {                       /* acabou sem '}' */
                registrarErro("LEXICO", linhaInicio, colunaInicio, "{",
                              "comentario nao fechado");
                erros++;
            } else {
                avancar();                               /* pula o '}' */
            }
        }

        /* ---- Comentario com parenteses e asterisco: (* ... *) ---- */
        else if (c == '(' && proximo() == '*') {
            avancar();                                   /* pula o '(' */
            avancar();                                   /* pula o '*' */
            while (atual() != '\0' && !(atual() == '*' && proximo() == ')')) {
                avancar();
            }
            if (atual() == '\0') {
                registrarErro("LEXICO", linhaInicio, colunaInicio, "(*",
                              "comentario nao fechado");
                erros++;
            } else {
                avancar();                               /* pula o '*' */
                avancar();                               /* pula o ')' */
            }
        }

        /* ---- Identificador ou palavra reservada ----
           Automato: (letra | _) seguido de (letra | digito | _)* */
        else if (isalpha((unsigned char)c) || c == '_') {
            i = 0;
            while (isalnum((unsigned char)atual()) || atual() == '_') {
                /* tolower: Pascal nao diferencia maiusculas de minusculas */
                if (i < 99) lexema[i++] = (char)tolower((unsigned char)atual());
                avancar();
            }
            lexema[i] = '\0';

            /* So depois de ler a palavra inteira decidimos o que ela e.
               Assim "beginx" vira um identificador (maximal munch). */
            if (ehPalavraReservada(lexema)) {
                adicionarToken(lista, lexema, TK_PR, linhaInicio, colunaInicio);
            } else {
                adicionarToken(lista, lexema, TK_ID, linhaInicio, colunaInicio);
            }
        }

        /* ---- Numero inteiro ou real ----
           Automato: digito+ ( . digito+ )? */
        else if (isdigit((unsigned char)c)) {
            int ehReal = 0;
            i = 0;
            while (isdigit((unsigned char)atual())) {
                if (i < 99) lexema[i++] = atual();
                avancar();
            }
            /* So vira real se depois do ponto vier um digito.
               Em "10." o ponto fica para ser um delimitador. */
            if (atual() == '.' && isdigit((unsigned char)proximo())) {
                ehReal = 1;
                if (i < 99) lexema[i++] = '.';
                avancar();
                while (isdigit((unsigned char)atual())) {
                    if (i < 99) lexema[i++] = atual();
                    avancar();
                }
            }

            /* Numero colado em letras, como "12abc", nao e token valido */
            if (isalpha((unsigned char)atual()) || atual() == '_') {
                while (isalnum((unsigned char)atual()) || atual() == '_') {
                    if (i < 99) lexema[i++] = atual();
                    avancar();
                }
                lexema[i] = '\0';
                registrarErro("LEXICO", linhaInicio, colunaInicio, lexema,
                              "numero malformado (numero seguido de letras)");
                erros++;
            } else {
                lexema[i] = '\0';
                adicionarToken(lista, lexema, ehReal ? TK_NUM_REAL : TK_NUM_INT,
                               linhaInicio, colunaInicio);
            }
        }

        /* ---- Literal de texto: 'ola mundo' ---- */
        else if (c == '\'') {
            i = 0;
            lexema[i++] = '\'';
            avancar();                                   /* pula o ' inicial */
            while (atual() != '\'' && atual() != '\n' && atual() != '\0') {
                if (i < 97) lexema[i++] = atual();       /* deixa espaco para ' e \0 */
                avancar();
            }
            if (atual() == '\'') {
                lexema[i++] = '\'';
                lexema[i] = '\0';
                avancar();                               /* pula o ' final */
                adicionarToken(lista, lexema, TK_TEXTO, linhaInicio, colunaInicio);
            } else {                                     /* a linha acabou antes */
                lexema[i] = '\0';
                registrarErro("LEXICO", linhaInicio, colunaInicio, lexema,
                              "literal de texto nao fechado");
                erros++;
            }
        }

        /* ---- Operadores de dois caracteres: := <= >= <> ---- */
        else if ((c == ':' && proximo() == '=') ||
                 (c == '<' && (proximo() == '=' || proximo() == '>')) ||
                 (c == '>' && proximo() == '=')) {
            lexema[0] = c;
            lexema[1] = proximo();
            lexema[2] = '\0';
            avancar();
            avancar();
            adicionarToken(lista, lexema, TK_OP, linhaInicio, colunaInicio);
        }

        /* ---- Operadores de um caractere: + - * / = < > ---- */
        else if (strchr("+-*/=<>", c) != NULL) {
            lexema[0] = c;
            lexema[1] = '\0';
            avancar();
            adicionarToken(lista, lexema, TK_OP, linhaInicio, colunaInicio);
        }

        /* ---- Delimitadores: ; : , . ( ) ---- */
        else if (strchr(";:,.()", c) != NULL) {
            lexema[0] = c;
            lexema[1] = '\0';
            avancar();
            adicionarToken(lista, lexema, TK_DELIM, linhaInicio, colunaInicio);
        }

        /* ---- Qualquer outra coisa: caractere fora da linguagem ---- */
        else {
            lexema[0] = c;
            lexema[1] = '\0';
            avancar();
            registrarErro("LEXICO", linhaInicio, colunaInicio, lexema,
                          "caractere invalido (nao pertence a linguagem)");
            erros++;
        }
    }

    /* Um token especial marca o fim. Assim o parser nunca le alem da lista. */
    adicionarToken(lista, "fim do arquivo", TK_EOF, linha, coluna);

    free(fonte);
    return erros;
}

const char *nomeTipoToken(TipoToken tipo) {
    switch (tipo) {
        case TK_PR:       return "PR";
        case TK_ID:       return "ID";
        case TK_NUM_INT:  return "NUM_INT";
        case TK_NUM_REAL: return "NUM_REAL";
        case TK_TEXTO:    return "TEXTO";
        case TK_OP:       return "OP";
        case TK_DELIM:    return "DELIM";
        case TK_EOF:      return "EOF";
    }
    return "?";
}

void imprimirTabelaTokens(const ListaTokens *lista) {
    printf("\nTABELA DE SIMBOLOS INICIAL (tokens)\n");
    printf("%-20s %-10s %-6s %s\n", "LEXEMA", "TOKEN", "LINHA", "COLUNA");
    for (int k = 0; k < lista->total; k++) {
        const Token *t = &lista->itens[k];
        if (t->tipo == TK_EOF) continue;   /* o marcador de fim nao aparece */
        printf("%-20s %-10s %-6d %d\n", t->lexema, nomeTipoToken(t->tipo),
               t->linha, t->coluna);
    }
}

void liberarTokens(ListaTokens *lista) {
    free(lista->itens);
    lista->itens = NULL;
    lista->total = 0;
    lista->capacidade = 0;
}
