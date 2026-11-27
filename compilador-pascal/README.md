# Compilador Pascal (subconjunto) em C

Analisador para um subconjunto da linguagem Pascal, feito em C (C99) para a disciplina de Compiladores — Universidade FUMEC.

# Integrantes do Grupo

- Igor Fagundes Martins
- Aline Aparecida Fernandes Madeira
- Gustavo Santos Simões

## Andamento

- [x] Etapa 1: análise léxica
- [ ] Etapa 2: análise sintática e árvore sintática abstrata (AST)
- [ ] Etapa 3: análise semântica

## Como compilar e executar

```bash
make                               # ou: mingw32-make (Windows com MinGW)
./compilador testes/valido.pas     # Windows: .\compilador.exe testes\valido.pas
```

Sem o `make`:

```bash
gcc -std=c99 -Wall -Wextra -o compilador main.c lexer.c erros.c
```

## Estrutura atual

| Arquivo | Responsabilidade |
|---|---|
| `main.c` | Abre o arquivo recebido por argumento e chama a análise léxica |
| `lexer.c` / `lexer.h` | Análise léxica: transforma os caracteres em tokens |
| `erros.c` / `erros.h` | Tabela de erros, compartilhada pelas fases |
| `testes/` | Arquivos-fonte de teste |

## Tokens reconhecidos

- Palavras reservadas, identificadores
- Literais numéricos (inteiros e reais) e literais de texto
- Operadores (`+ - * / := = <> < <= > >=`) e delimitadores (`; : , . ( )`)
- Comentários `{ ... }` e `(* ... *)`, que são descartados

Erros léxicos detectados: caractere inválido, número malformado, literal de texto não fechado e comentário não fechado.
