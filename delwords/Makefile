CC=gcc
CFLAGS=-pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -D_FORTIFY_SOURCE=2 -g
TARGET=delwords
LDFLAGS=-I../lib/ -L../lib/ -Wl,-rpath=../lib/
LIBS=-lhelpers

$(TARGET): delwords.c
	$(CC) $(LDFLAGS) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm $(TARGET)
