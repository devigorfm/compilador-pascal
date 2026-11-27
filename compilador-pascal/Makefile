CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g

OBJETOS = main.o lexer.o erros.o

# Regra principal: junta todos os .o no executavel
compilador: $(OBJETOS)
	$(CC) $(CFLAGS) -o compilador $(OBJETOS)

# Cada .c vira um .o
main.o: main.c lexer.h erros.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h erros.h
	$(CC) $(CFLAGS) -c lexer.c

erros.o: erros.c erros.h
	$(CC) $(CFLAGS) -c erros.c

# make clean: apaga os arquivos gerados na compilacao
clean:
	rm -f $(OBJETOS) compilador compilador.exe

.PHONY: clean
