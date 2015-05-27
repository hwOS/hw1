CC=gcc
CFLAGS=-std=c11 -g
LDFLAGS=-L../lib -I../lib -Wl,-rpath,../lib/
LIBS=-lhelpers
TARGET=simplesh

$(TARGET): simplesh.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(LIBS) -o $@

clean:
	rm -rf $(TARGET)
