CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -c
CSTD=-std=c11
SRC=src/main.c src/misc.c
OBJ=main.o misc.o
TARGET=ams

all: link

.PHONEY:
link: compile
	$(CC) -o $(TARGET) $(OBJ)

.PHONEY:
compile: $(SRC)
	$(CC) $(CSTD) $(CFLAGS) $(SRC)
