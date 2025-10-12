CC = clang
CFLAGS = -fcolor-diagnostics -fansi-escape-codes -Wall -g -Iinclude

CFLAGS += $(OPT_CFLAGS)

SRC = $(wildcard src/*.c)

OBJ = $(SRC:.c=.o)

EXEC = csh

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
