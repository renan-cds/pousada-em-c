# Compilacao no estilo da disciplina (gcc + um unico .c)
CC = gcc
CFLAGS = -Wall -Wextra
BIN = pousada

.PHONY: all clean run test

all: $(BIN)

$(BIN): pousada.c
	$(CC) $(CFLAGS) -o $(BIN) pousada.c

run: $(BIN)
	./$(BIN)

# Teste automatizado da logica de datas/disponibilidade (arquivo unico)
test: test_disponibilidade
	./test_disponibilidade

test_disponibilidade: tests/test_disponibilidade.c
	$(CC) $(CFLAGS) -o test_disponibilidade tests/test_disponibilidade.c

clean:
	rm -f $(BIN) test_disponibilidade *.dat *.tmp
