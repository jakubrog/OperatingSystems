CC = gcc -std=c11 -Wall
FILE = results
TEST_DEST = /home

compile:
	$(CC) -o main main.c -O2

tests:
	./main $(TEST_DEST) '>' "2019-03-25 18:25"

clean:
	rm -rf main
