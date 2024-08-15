all: main.o transitions.o
	gcc -g main.o transitions.o -o all -lm

main.o: Main.c transitions.h
	gcc -c Main.c -o main.o

transition.o: transitions.c transitions.h
	gcc -c transitions.c -o transitions -lm
clean: rm *~
