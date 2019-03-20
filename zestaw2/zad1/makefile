CC = gcc -std=c11 -Wall
FILE = wyniki.txt

tests:
	$(CC) -o main main.c -O2
	for l in 1 4 512 1024 4096 8192; \
	do \
				for a in 1000 10000; \
				do \
							echo "Creating file with $$a records of size $$l: " | tee -a $(FILE); \
							./main generate test_file $$a $$l; \
							for m in sys lib; \
							do \
										echo "$$m sorting file with $$a records of size $$l" | tee -a $(FILE); \
										cp test_file test_file_temp; \
										./main sort test_file_temp $$a $$l $$m | tee -a $(FILE); \
										rm test_file_temp; \
										echo "$$m copy file with $$a records of size $$l" | tee -a $(FILE); \
										./main copy test_file test_copy $$a $$l $$m | tee -a $(FILE); \
							done; \
							echo " " | tee -a $(FILE); \
							rm test_file; \
				done; \
		done; \
