CC = gcc -std=gnu99 -Wall
FILE = results

compile:
	$(CC) -o main main.c -O2


clean:
	rm -rf main commands

tests:
	touch commands
	echo "ls -al | grep jakubrog | grep makefile" >> commands

	./main commands
