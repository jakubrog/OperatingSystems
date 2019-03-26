CC = gcc -std=c11 -Wall
FILE = results

compile:
	$(CC) -o main main.c -O2
	$(CC) -o tester tester.c -O2

tests: compile
	touch list
	echo -e "$(PWD)/test1 2\n$(PWD)/test2 1" > list
	touch test1 test2
	./main $(PWD)/list 15 copy
	./tester $(PWD)/test1 1 4 10
	./tester $(PWD)/test2 1 10 20
	./main $(PWD)/list 15 nocopy 100 10000
	./main $(PWD)/list 15 copy 100 10000
	rm test1 test2

clean:
	rm -rf archive main tester test1 test2 list
