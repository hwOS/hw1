CC=gcc
CFLAGS=-pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op  -D_FORTIFY_SOURCE=2 -g -fPIC 
LDFLAGS=-shared
TARGET=helpers.so

all: $(TARGET)

$(TARGET) : herlpers.o
	$(CC) $(LDFLAGS) $^ -o $@

%.o: helpers.c helpers.h
	$(CC) $(CFLAGS) $< -o $@
