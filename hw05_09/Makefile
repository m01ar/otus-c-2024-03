OPT = -std=c11 -Wall -Werror -Wextra -Wpedantic -Wconversion 

all: asm-prog-in-c

asm-prog-in-c: main.c
	$(CC) $(OPT) $^ -o $@

clean:
	rm -f asm-prog-in-c *.o

.PHONY: clean
