CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c11 -O0 -ggdb -no-pie -lm
LDFLAGS= -lm 

# Lista de archivos 
SOURCES = cachesim.c verboso.c cache_funcs.c aux_lib.c

all: cachesim

cachesim: $(SOURCES) cachesim.h
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o
	rm -f cachesim
	

