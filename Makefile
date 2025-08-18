CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -pthread
SRC = src/main.c src/server.c src/config.c src/log.c src/pwd.c src/crypt.c src/session.c src/tui.c src/socket.c src/udp.c src/user.c src/util.c
OBJ = $(SRC:.c=.o)
EXEC = fileflow

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
