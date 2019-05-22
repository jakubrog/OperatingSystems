CC = gcc -Wall -std=gnu99 -ggdb -lpthread -lm
IMAGE = check.pgm
GEN = generate_filter
FIL = filter
FILTER_FILE = fil.pgm
TIMES_FILE = times.txt
RESULT_FILE = result.pgm

compile:
	$(CC) filter.c -o $(FIL)
	$(CC) $(GEN).c -o $(GEN)

clean:
	rm -rf $(GEN) $(FIL) $(RESULT_FILE) $(FILTER_FILE)

test:
	for filter_size in 7 20 40 55; do \
			./$(GEN) $(FILTER_FILE) $$filter_size ; \
			for threads in 1 2 4 8; do \
						for mode in interleaved block; do \
						 		./$(FIL) $$threads $(IMAGE) $(FILTER_FILE) $$mode $(RESULT_FILE) | tee -a $(TIMES_FILE); \
						done ; \
			done ; \
			rm $(FILTER_FILE) ; \
	done
