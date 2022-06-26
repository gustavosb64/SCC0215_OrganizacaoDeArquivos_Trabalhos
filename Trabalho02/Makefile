all:
	lua ../autotad.lua records_utils.c index.c reg_type1.c reg_type2.c 
	gcc -g -Wall -Wpedantic -o main *.c

c:
	gcc -g -o main *.c

run:
	./main

debug:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./main
