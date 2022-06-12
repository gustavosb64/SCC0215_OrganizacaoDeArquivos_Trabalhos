all:
	lua ../autotad.lua functions.c
	gcc -g -Wall -Wpedantic -o main main.c functions.c

c:
	gcc -g -o main main.c functions.c

run:
	./main

debug:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./main
