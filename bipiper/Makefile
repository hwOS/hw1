CC=gcc
TARGET=forking
CFLAGS=-std=c11 -Wall -Wno-unused-parameter -I../lib -L../lib -Wl,-rpath=../lib -D_POSIX_C_SOURCE=199309L
LIBS=-lbufio

$(TARGET): forking.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -rf $(TARGET)
