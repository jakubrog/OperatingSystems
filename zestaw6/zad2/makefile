CC = gcc
FLAGS = -Wall -std=gnu99 -lrt


compile:
	$(CC) $(FLAGS) server.c -o server
	$(CC) $(FLAGS) client.c -o client

clean:
	rm -f server client *.o
