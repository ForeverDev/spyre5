CC = g++
CF = -std=c++11
DEPS = build/main.o build/lex.o build/parse.o

all: spy

spy: build $(DEPS)
	$(CC) $(DEPS) $(CF) -o spy 
	mv spy /usr/local/bin
	rm -Rf build/*

build:
	mkdir build

build/main.o:
	$(CC) $(CF) -c src/main.cpp -o build/main.o

build/lex.o:
	$(CC) $(CF) -c src/lex.cpp -o build/lex.o

build/parse.o:
	$(CC) $(CF) -c src/parse.cpp -o build/parse.o
