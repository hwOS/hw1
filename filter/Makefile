CC=gcc
CFLAGS=-pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -D_FORTIFY_SOURCE=2 -g
TARGET=filter
LDFLAGS=-I../lib/ -I. -L../lib/ -Wl,-rpath=../lib/
LIBS=-lhelpers

$(TARGET): filter.c
	$(CC) $(LDFLAGS) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(TARGET)
