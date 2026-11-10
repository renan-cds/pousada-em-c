# Makefile auxiliar.
# Entrega / uso principal: Windows (Dev-C++).
#   gcc -Wall -o pousada.exe pousada.c
#
# Neste ambiente de CI (Linux) usamos um stub local de conio.h
# apenas para compilar/testar — o arquivo entregue continua Windows.

CC = gcc
CFLAGS = -Wall -Wextra -I.win_conio_stub
BIN = pousada

.PHONY: all clean run test stub

all: stub $(BIN)

stub:
	@mkdir -p .win_conio_stub
	@printf '%s\n' \
	  '/* Stub local so para compilar testes em Linux. Nao usar no Windows. */' \
	  '#ifndef CONIO_H' \
	  '#define CONIO_H' \
	  '#include <stdio.h>' \
	  'static int getch(void) { int c = getchar(); return c; }' \
	  '#endif' \
	  > .win_conio_stub/conio.h

$(BIN): pousada.c stub
	$(CC) $(CFLAGS) -o $(BIN) pousada.c

run: $(BIN)
	./$(BIN)

test: stub
	$(CC) -Wall -Wextra -o test_disponibilidade tests/test_disponibilidade.c
	./test_disponibilidade

clean:
	rm -f $(BIN) test_disponibilidade *.dat *.tmp
	rm -rf .win_conio_stub
