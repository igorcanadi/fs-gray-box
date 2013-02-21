CC=gcc
CFLAGS=-O1 -g -D _GNU_SOURCE
BINFOLDER=./bin

all: p1 p2 p3 p4

p1:	p1.c
	$(CC) $(CFLAGS) -o $(BINFOLDER)/p1 p1.c

p2:	p2.c
	$(CC) $(CFLAGS) -o $(BINFOLDER)/p2 p2.c

p3:	p3.c
	$(CC) $(CFLAGS) -o $(BINFOLDER)/p3 p3.c

p4:	p4.c
	$(CC) $(CFLAGS) -o $(BINFOLDER)/p4 p4.c

clean:
	rm -f *.o $(BINFOLDER)/*
