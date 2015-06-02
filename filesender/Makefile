CC=gcc
TARGET=filesender
CFLAGS=-g -std=c11 -Wall -I../lib -L../lib -Wl,-rpath=../lib -D_POSIX_C_SOURCE=199309L 
LIBS=-lbufio

$(TARGET): filesender.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@ 

clean:
	rm -rf $(TARGET)
