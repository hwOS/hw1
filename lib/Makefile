CC=gcc
CFLAGS=-pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op  -D_FORTIFY_SOURCE=2 -g -fPIC -c
LDFLAGS=-shared
TARGET=libhelpers.so
OBJECTS=helpers.o

$(TARGET) : $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
	rm $(OBJECTS)

$(OBJECTS): helpers.c helpers.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(TARGET)
