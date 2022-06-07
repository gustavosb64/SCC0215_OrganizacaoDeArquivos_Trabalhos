all:
	python ../autotad.py records.c index.c lista_encadeada.c
	gcc -g -Wall -Wpedantic -o main *.c

c:
	gcc -g -o main *.c

run:
	./main

debug:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./main
