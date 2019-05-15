CC = gcc -Wall -std=gnu99 -ggdb -lpthread -lrt -lm

compile:
	$(CC) filter.c -o filter
	$(CC) generate_filter.c -o gener

clean:
	rm -rf filter gener
