CC = gcc -std=gnu99 -Wall
FILE = results

compile:
	$(CC) -o main main.c -O2


clean:
	rm -rf main

tests:
	touch commands
	echo "ls -al | grep Apr | tr a-z A-Z | grep MAKEFILE" >> commands
	./main commands
	rm commands
