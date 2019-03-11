CC = gcc -Wall -std=c11 -ggdb
MAIN_SRC = main
FILE = raport2.txt

all: static shared clean

static:
	$(CC) -c library.c
	ar rcs library.a library.o
	$(CC) $(MAIN_SRC).c -I. library.a -o $(MAIN_SRC)_static

shared:
	$(CC) -c -fPIC library.c -o library.o
	$(CC) -shared -fPIC -o library.so library.o
	$(CC) -L. library.so -o $(MAIN_SRC)_shared $(MAIN_SRC).c

clean:
	rm -f *.o

tests:
		echo 'Static library' | tee -a $(FILE)
		echo 'Different levels of depth' | tee -a $(FILE)
		echo ' ' | tee -a $(FILE)
		echo 'Shallow' | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/home" \"resultFile*\" resultFile1 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/home" \"resultFile1\" resultFile2 remove_block 0 | tee -a $(FILE)
		echo ' ' | tee -a $(FILE)
		echo 'Deeper' | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr/local" \"*\" resultFile3 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr/local" \"*a*\" resultFile4 remove_block 0 | tee -a $(FILE)
		echo ' ' | tee -a $(FILE)
		echo 'Deeper' | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/home" \"*\" resultFile5 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/home" \"*.txt\" resultFile6 remove_block 0 | tee -a $(FILE)
		echo ' ' | tee -a $(FILE)
		echo 'Very deep' | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr" \"*.txt\" resultFile7 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr" \"*\" resultFile8 remove_block 0 | tee -a $(FILE)
		echo ' ' | tee -a $(FILE)
		echo 'Adding and removing, first two very deep, third swallow' | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr" \"*.txt\" resultFile9 remove_block 0 search_directory "/usr" \"*\"	resultFile10 remove_block 0 search_directory "/home" \"*.txt\" resultFile11 remove_block 0 | tee -a $(FILE)
