CC=afl-cc
CFLAGS=-I/usr/include/libxml2 -lxml2

.PHONY: all
all: harness

harness: harness.c
	$(CC) -o $@ $< $(CFLAGS)
