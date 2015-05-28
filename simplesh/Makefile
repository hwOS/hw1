CC=gcc
CFLAGS=-std=c11 -g -D_POSIX_C_SOURCE=199309L 
LDFLAGS=-L../lib -I../lib -Wl,-rpath,../lib/
LIBS=-lhelpers -lbufio
TARGET=simplesh

$(TARGET): simplesh.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(LIBS) -o $@

clean:
	rm -rf $(TARGET)
