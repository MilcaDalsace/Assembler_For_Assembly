all: main preAssembler
	gcc -g -Wall -pedantic -ansi -std=c99 main preAssembler -o all -lm
main: Main.c preAssembler.h
	gcc -c -Wall -pedantic -ansi -std=c99 Main.c -o main
preAssembler: preAssembler.c preAssembler.h
	gcc -c -Wall -pedantic -ansi -std=c99 preAssembler.c -o preAssembler -lm
clean: rm *~