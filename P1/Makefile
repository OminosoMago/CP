CC=gcc
CFLAGS= -g -o

all:main

main:main.c
	$(CC) $(CFLAGS) main main.c

clean:
	rm main

run:main
	./main
valgrind: main
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./main

