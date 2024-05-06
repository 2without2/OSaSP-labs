CC=gcc
CFLAGC = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

all:
	$(CC) $(CFLAGC) parent.c -o parent
	$(CC) $(CFLAGC) child.c -o child
clean:
	rm -rf *.o parent
	rm -rf *.o child
