CC=gcc
FORKING=forking
POLLING=polling
TARGET=$(FORKING) $(POLLING)
CFLAGS=-std=c11 -Wall -Wno-unused-parameter -I../lib -L../lib -Wl,-rpath=../lib -D_POSIX_C_SOURCE=199309L
LIBS=-lbufio

all: $(TARGET)

$(FORKING): forking.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

$(POLLING): polling.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -rf $(TARGET)
