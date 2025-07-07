CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c99 -pthread
SRC = src/main.c src/server.c src/config.c src/log.c src/pwd.c src/crypt.c 
OBJ = $(SRC:.c=.o)
EXEC = fileflow

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
