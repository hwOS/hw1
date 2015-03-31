#ifndef BUFIO_H
#define BUFIO_H
#include <unistd.h>


struct buf_t {
    char* data;
    size_t size;
    size_t capacity;
};

/* construct empty buffer, return NULL if couldn't do malloc */
struct buf_t *buf_new(size_t capacity);
void buf_free(struct buf_t *);
size_t buf_capacity(struct buf_t*); 
size_t buf_size(struct buf_t*);

/*
 * fill the buffer bytes read from fd, at lest required bytes
 * on error -1 is returned, otherwise return size of buf
 */
ssize_t buf_fill(int fd, struct buf_t *buf, size_t required);

/*
 * flush at least required bytes from buf to fd, flushed bytes remove from buf
 * on error -1 is returned, otherwise return count of flushed bytes
 */
ssize_t buf_flush(int fd, struct buf_t *buf, size_t required);

#endif
