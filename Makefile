CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude
LDFLAGS =

SRC_DIR = src
OBJ_DIR = obj
BIN = pousada

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean run dirs test

all: dirs $(BIN)

dirs:
	@mkdir -p $(OBJ_DIR) data

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(BIN)

TEST_BIN = test_disponibilidade
TEST_OBJS = $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

test: dirs $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_BIN) tests/test_disponibilidade.c $(TEST_OBJS) $(LDFLAGS)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR) $(BIN) $(TEST_BIN)
