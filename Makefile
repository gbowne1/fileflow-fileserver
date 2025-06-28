CC = gcc
CFLAGS = -Wall -g
SRC = src/main.c src/server.c
OBJ = $(SRC:.c=.o)
EXEC = file_server

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
