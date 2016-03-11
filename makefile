CC = gcc
CF = -std=c11
DEPS = build/main.o build/lex.o

all: spy

spy: build $(DEPS)
	$(CC) $(DEPS) $(CF) -o spy 
	mv spy /usr/local/bin
	rm -Rf build/*

build:
	mkdir build

build/main.o:
	$(CC) $(CF) -c src/main.c -o build/main.o

build/lex.o:
	$(CC) $(CF) -c src/lex.c -o build/lex.o
