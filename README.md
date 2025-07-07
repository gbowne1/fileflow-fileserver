# fileflow-fileserver
This is FileFlow, a cross platform TCP file server written in C

# Compile

`gcc -std=c99 -Wall -Wextra -pedantic -pthread -o fileflow main.c server.c pwd.c log.c crypt.c config.c`

Use the `Makefile` if you have `make` installed.  
