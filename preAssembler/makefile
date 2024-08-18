all: main preAssembler
	gcc -g -Wall -pedantic -ansi main preAssembler -o all -lm
main: Main.c preAssembler.h
	gcc -c -Wall -pedantic -ansi Main.c -o main
preAssembler: preAssembler.c preAssembler.h
	gcc -c -Wall -pedantic -ansi preAssembler.c -o preAssembler -lm
clean: rm *~
