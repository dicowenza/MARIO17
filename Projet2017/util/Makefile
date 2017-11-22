CC = gcc
CFLAGS := -g -std=c99 -Wall -Wno-unused-function
	
all: maputil
maputil: maputil.o function.o
function.o : function.c function.h
maputil.o: maputil.c function.h

.PHONY: clean
clean:
	rm -f maputil.o
	rm -f maputil
