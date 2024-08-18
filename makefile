
all: main transition functions
	gcc -g main transition functions -o all -lm

main: main.c transition.h function.h
	gcc -c main.c -o main

transition: transition.c transition.h function.h
	gcc -c transition.c -o transition -lm

functions: function.c function.h
	gcc -c function.c -o functions -lm
clean: rm *~


