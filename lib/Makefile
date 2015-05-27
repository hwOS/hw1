CC=gcc
CFLAGS=-std=c11 -pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 \
-Wfloat-equal -Wconversion -Wlogical-op -D_FORTIFY_SOURCE=2 -g -shared -fPIC -DDEBUG -D_POSIX_C_SOURCE=199309L 

HELPERS=libhelpers.so
BUFIO=libbufio.so

TARGET=$(BUFIO) $(HELPERS)

all: $(TARGET)
	
$(BUFIO) : bufio.c bufio.h
	$(CC) $(CFLAGS) $< -o $@

$(HELPERS) : helpers.c helpers.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
