all: main transition functions preAssembler
	gcc -g main transition functions preAssembler -o all -lm

transition: transition.c transition.h function.h
	gcc -c transition.c -o transition -lm

functions: function.c function.h
	gcc -c function.c -o functions -lm

preAssembler: preAssembler.c preAssembler.h function.h
	gcc -c preAssembler.c -o preAssembler -lm

main: main.c transition.h preAssembler.h function.h
	gcc -c main.c -o main

clean: rm *~