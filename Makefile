CC=gcc
CFLAGS=-O1 -g

all: p1 p2 p3

p1:	p1.c
	$(CC) $(CFLAGS) -o p1 p1.c

p2:	p2.c
	$(CC) $(CFLAGS) -o p2 p2.c

p3:	p3.c
	$(CC) $(CLAGS) -o p3 p3.c
clean:
	rm -f *.o p1 p2 p3
