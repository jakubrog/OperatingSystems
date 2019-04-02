CC = gcc -D_POSIX_SOURCE -std=c11 -Wall
FILE = results

compile:
	$(CC) -o monitor monitor.c -O2


tests: compile
	touch list
	echo -e "$(PWD)/test1 2\n$(PWD)/test2 1" > list
	touch test1 test2
	./monitor $(PWD)/list
	rm test1 test2

clean:
	rm -rf archive main tester test1 test2 list monitor
