CC=gcc
CFLAGS=-std=c11 -pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 \
-Wfloat-equal -Wconversion -Wlogical-op  -D_FORTIFY_SOURCE=2 -g -fPIC -c -DDEBUG
LDFLAGS=-shared

HELPERS=libhelpers.so
BUFIO=libbufio.so

TARGET=$(BUFIO) $(HELPERS)

all: $(BUFIO) $(HELPERS) 
	
$(BUFIO) : bufio.c bufio.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(HELPERS) : helpers.c helpers.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
