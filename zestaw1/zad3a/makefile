CC = gcc -Wall -std=c11 -ggdb
MAIN_SRC = main
FILE = results3a.txt

all: static shared dynamic clean

static:
	$(CC) -c library.c
	ar rcs library.a library.o
	$(CC) $(MAIN_SRC).c -I. library.a -o $(MAIN_SRC)_static


dynamic:
	$(CC) -c -fPIC library.c -o library.o
	$(CC) -shared -fPIC -o library.so library.o
	$(CC) -Wl,-rpath=. -L. -o $(MAIN_SRC)_dynamic $(MAIN_SRC)_dynamic.c -ldl -D DLL


shared:
	$(CC) -c -fPIC library.c -o library.o
	$(CC) -shared -fPIC -o library.so library.o
	$(CC) -Wl,-rpath=. -L. library.so -o $(MAIN_SRC)_shared $(MAIN_SRC).c



clean:
	rm -f *.o

tests:
		echo 'static library' | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr" \"*.txt\" resultFile1 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr/local/bin" \"*.txt\" resultFile2 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/home" \"*.txt\" resultFile3 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr" \"*.txt\" resultFile4 remove_block 0 search_directory "/usr" \"*\"	resultFile5 remove_block 0 search_directory "/home" \"*.txt\" resultFile6 remove_block 0 | tee -a $(FILE)
		echo ' ' | tee -a $(FILE)
		echo 'shared library' | tee -a $(FILE)
		./$(MAIN_SRC)_shared create_table 10000000 search_directory "/usr" \"*.txt\" resultFile7 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_shared create_table 10000000 search_directory "/usr/local/bin" \"*.txt\" resultFile8 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_shared create_table 10000000 search_directory "/home" \"*.txt\" resultFile9 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_shared create_table 10000000 search_directory "/usr" \"*.txt\" resultFile10 remove_block 0 search_directory "/usr" \"*\"	resultFile11 remove_block 0 search_directory "/home" \"*.txt\" resultFile 12 remove_block 0 | tee -a $(FILE)
		echo ' ' | tee -a $(FILE)
		echo 'Dynamic library' | tee -a $(FILE)
		./$(MAIN_SRC)_dynamic create_table 10000000 search_directory "/usr" \"*.txt\" resultFile13 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_dynamic create_table 10000000 search_directory "/usr/local/bin" \"*.txt\" resultFile14 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_dynamic create_table 10000000 search_directory "/home" \"*.txt\" resultFile15 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_dynamic create_table 10000000 search_directory "/usr" \"*.txt\" resultFile16 remove_block 0 search_directory "/usr" \"*\"	resultFile17 remove_block 0 search_directory "/home" \"*.txt\" resultFile18 remove_block 0 | tee -a $(FILE)
