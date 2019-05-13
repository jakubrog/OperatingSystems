CC = gcc -Wall -std=c11 -ggdb


main:
	$(CC) -c load.c
	ar rcs load.a load.o
	$(CC) loader.c -I. load.a -o loader
	$(CC) truck.c -I. load.a -o truck

clean:
	rm -rf load.a load.o loader truck
