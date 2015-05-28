#ifndef BUFIO_H
#define BUFIO_H
#include <unistd.h>
#include <sys/types.h>

typedef struct {
    char* data;
    size_t size;
    size_t capacity;
} buf_t;

/* construct empty buffer, return NULL if couldn't do malloc */
buf_t *buf_new(size_t capacity);
void buf_free(buf_t *);
size_t buf_capacity(buf_t*); 
size_t buf_size(buf_t*);

/*
 * fill the buffer bytes read from fd, at lest required bytes
 * on error -1 is returned, otherwise return new size of buf
 */
ssize_t buf_fill(int fd, buf_t *buf, size_t required);

/*
 * flush at least required bytes from buf to fd, flushed bytes remove from buf
 * on error -1 is returned, otherwise return count of flushed bytes
 */
ssize_t buf_flush(int fd, buf_t *buf, size_t required);

void buf_clear(buf_t *buf);

#endif
