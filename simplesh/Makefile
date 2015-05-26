CC=gcc
CFLAGS=-std=c11 -g
LDFLAGS=-lhelpers -L../lib -I../lib 
TARGET=simplesh

$(TARGET): simplesh.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET)
