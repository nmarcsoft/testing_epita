CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
OBJ = src/main.o src/action.o src/decode_and_log.o

TEST_FILES = ./tests/test_decode.c

all: main

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)

main.o: main.c global.h action.h decode_and_log.h
	$(CC) $(CFLAGS) -c main.c

action.o: action.c action.h global.h
	$(CC) $(CFLAGS) -c action.c

decode_and_log.o: decode_and_log.c decode_and_log.h global.h
	$(CC) $(CFLAGS) -c decode_and_log.c

clean:
	rm -f *.o main

distclean: clean
	rm -f main

test: $(TEST_FILES) src/action.o src/decode_and_log.o
	$(CC) $(CFLAGS) -o $@ $^ -lcunit
