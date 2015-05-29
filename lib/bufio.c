#include <stdio.h>
#include "bufio.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef DEBUG
    #define check_buf(buf) assert(buf != NULL);
#else
    #define check_buf(buf)
#endif

buf_t *buf_new(size_t capacity) {
    buf_t *res_buf = (buf_t *) malloc(sizeof(buf_t));
    check_buf(res_buf);
    res_buf->data = (char*) malloc(capacity);
#ifdef DEBUG
    assert(capacity == 0 || res_buf->data != NULL);
#endif
    res_buf->capacity = capacity;
    res_buf->size = 0;
    return res_buf;
}

void buf_free(buf_t *buf) {
    check_buf(buf);
    free(buf->data);
    free(buf);
}

size_t buf_capacity(buf_t *buf) {
    check_buf(buf);
    return buf->capacity;
}

size_t buf_size(buf_t *buf) {
    check_buf(buf);
    return buf->size;
}

ssize_t buf_fill(int fd, buf_t *buf, size_t required) {
#ifdef DEBUG
    assert(buf != NULL);
    assert(buf->capacity - buf->size >= required);
#endif
    ssize_t last_read_bytes;
    while (buf->size < required) {
        last_read_bytes = read(fd, buf->data + buf->size, buf->capacity - buf->size);
        if (last_read_bytes == 0) {
            return (ssize_t) buf->size;
        } else if (last_read_bytes == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        buf->size += (size_t) last_read_bytes;
    }
    return (ssize_t) buf->size;
}

static void move_data(buf_t *buf, size_t wrote_bytes, int fd) {
    buf->size = buf->size - wrote_bytes;
    memmove(buf->data, buf->data + wrote_bytes, buf->size);
    fsync(fd);
}

ssize_t buf_flush(int fd, buf_t *buf, size_t required) {
#ifdef DEBUG
    assert(buf != NULL);
    assert(buf->size >= required);
#endif
    ssize_t last_wrote_bytes;
    size_t wrote_bytes = 0;
    while (wrote_bytes < required) {
        last_wrote_bytes = write(fd, buf->data + wrote_bytes, buf->size - wrote_bytes);
        if (last_wrote_bytes == -1) {
            return -1;
        } else if (last_wrote_bytes == 0) {
            move_data(buf, wrote_bytes, fd);
            return (ssize_t) wrote_bytes;
        }
        wrote_bytes += (size_t) last_wrote_bytes;
    }
    move_data(buf, wrote_bytes, fd);
    return (ssize_t) wrote_bytes;
}

void buf_clear(buf_t *buf) {
    buf->size = 0;
}
