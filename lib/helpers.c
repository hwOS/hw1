#include "helpers.h"
#include <stdio.h>

ssize_t read_(int fd, void *buf, size_t count) {
    ssize_t num_bytes = 0;
    ssize_t read_bytes = 0;
    /* INV read_bytes, num_bytes <= count */
    while (count > 0) {
        read_bytes = read(fd, (char*) buf + num_bytes, count);
        if (read_bytes == -1) {
            return -1;
        } else if (read_bytes == 0) {
            return num_bytes;
        }
        count = count - (size_t) read_bytes;
        num_bytes += read_bytes;
    }
    return num_bytes;
}

ssize_t write_(int fd, const void *buf, size_t count) {
    size_t num_bytes = 0;
    int wrote_bytes = 0;
    /*INV wrote_bytes, num_bytes <= count*/
    while (count > 0) {
        wrote_bytes = write(fd, (char*) buf + num_bytes, count);
        if (wrote_bytes == -1) {
            return wrote_bytes;
        } else if (wrote_bytes == 0) {
            return num_bytes;
        }
        count -= wrote_bytes;
        num_bytes += wrote_bytes;
    }
    return 0;
}
