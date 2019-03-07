CC = gcc -Wall -std=c11 -ggdb
MAIN_SRC = main
FILE = results

compile: static shared dynamic

static:
	$(CC) -c library.c
	ar rcs library.a library.o
	$(CC) $(MAIN_SRC).c -I. library.a -o $(MAIN_SRC)_static
	make clean

dynamic:
	$(CC) -c -fPIC library.c -o library.o
	$(CC) -shared -fPIC -o library.so library.o
	$(CC) -Wl,-rpath=. -L. -o $(MAIN_SRC)_dynamic $(MAIN_SRC)_dynamic.c -ldl -D DLL
	make clean

shared:
	$(CC) -c -fPIC library.c -o library.o
	$(CC) -shared -fPIC -o library.so library.o
	$(CC) -Wl,-rpath=. -L. library.so -o $(MAIN_SRC)_shared $(MAIN_SRC).c
#	$(CC) -Wl,-rpath=. -L. -o $(MAIN_SRC)_dynamic $(MAIN_SRC)_dynamic.c -ldl -D DLL
	make clean


clean:
	rm -f *.o

tests:
		echo 'static library' >> $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr" \"*a\" resultFile1 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr" \"*abcdef\" resultFile2 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr/bin" \"aa*abcdef\" resultFile3 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_static create_table 10000000 search_directory "/usr" \"*abcdef\" resultFile4 remove_block 0 search_directory "/usr" \"*a\" resultFile5 remove_block 0 | tee -a $(FILE)
		echo 'shared library' >> $(FILE)
		./$(MAIN_SRC)_shared create_table 10000000 search_directory "/usr" \"*a\" resultFile1 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_shared create_table 10000000 search_directory "/usr" \"*abcdef\" resultFile2 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_shared create_table 10000000 search_directory "/usr/bin" \"aa*abcdef\" resultFile3 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_shared create_table 10000000 search_directory "/usr" \"*abcdef\" resultFile4 remove_block 0 search_directory "/usr" \"*a\" resultFile5 remove_block 0 | tee -a $(FILE)
		echo 'dynamic library' >> $(FILE)
		./$(MAIN_SRC)_dynamic create_table 10000000 search_directory "/usr" \"*a\" resultFile1 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_dynamic create_table 10000000 search_directory "/usr" \"*abcdef\" resultFile2 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_dynamic create_table 10000000 search_directory "/usr/bin" \"aa*abcdef\" resultFile3 remove_block 0 | tee -a $(FILE)
		./$(MAIN_SRC)_dynamic create_table 10000000 search_directory "/usr" \"*abcdef\" resultFile4 remove_block 0 search_directory "/usr" \"*a\" resultFile5 remove_block 0 | tee -a $(FILE)
