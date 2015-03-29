#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>

ssize_t read_(int fd, void *buf, size_t count) {
    size_t num_bytes = 0;
    ssize_t read_bytes = 0;
    /* INV read_bytes, num_bytes <= count */
    while (count > 0) {
        read_bytes = read(fd, (char*) buf + num_bytes, count);
        if (read_bytes == -1) {
            return -1;
        } else if (read_bytes == 0) {
            return (ssize_t) num_bytes;
        }
        count -= (size_t) read_bytes;
        num_bytes += (size_t) read_bytes;
    }
    return (ssize_t) num_bytes;
}

ssize_t write_(int fd, const void *buf, size_t count) {
    size_t num_bytes = 0;
    ssize_t wrote_bytes = 0;
    /* INV wrote_bytes, num_bytes <= count */
    while (count > 0) {
        wrote_bytes = write(fd, (char*) buf + num_bytes, count);
        if (wrote_bytes == -1) {
            return -1;
        }
        count -= (size_t) wrote_bytes;
        num_bytes += (size_t) wrote_bytes;
    }
    fsync(fd);
    return (ssize_t) num_bytes;
}

ssize_t find_delimiter(char* buf, size_t num_bytes, char delimiter) {
    size_t i;
    for (i = 0; i < num_bytes; ++i) {
        if (buf[i] == delimiter) {
            return (ssize_t) i;
        }
    }
    return -1;
}

static void move_str(char* to, char* from, size_t count, size_t* size_from, size_t size_to) {
    size_t i;
    if (size_to < (size_t) count) {
        fprintf(stderr, "count(%lu) < size of word(%lu)", size_to, count);
        abort();
    }

    for (i = 0; i < count; ++i) {
        to[i] = from[i];
    }
    to[count] = 0;

    count++; /* exclude delimiter */
    for (i = count; i <= *size_from; ++i) {
        from[i - count] = from[i];
    }
    if (count > *size_from) {
        *size_from = 0;
    } else {
        *size_from -= count;
    }
}

/*
 * read_until finish work if delimiter or EOF met,
 * or count byte was read.
 */
ssize_t read_until(int fd, void* buf, size_t count, char delimiter) {
    ssize_t i;
    ssize_t last_read_bytes = 0;
    size_t read_bytes = 0;

    while (read_bytes < count) {
        last_read_bytes = read(fd, buf + read_bytes, count - read_bytes);
        if (read_bytes == -1) {
            return -1;
        } else if (read_bytes == 0) {
            temp = size_local_buf;
            move_str(buf, local_buf, size_local_buf, &size_local_buf, count);
            return (ssize_t) temp;
        }
        size_local_buf += (size_t) read_bytes;
        if ((i = find_delimiter(local_buf, size_local_buf, delimiter)) != -1) {
            move_str(buf, local_buf, (size_t) i, &size_local_buf, count);
            return i;
        }
    }
    return -1;
}

char* get_word(char* buf) {

}

int spawn(const char * file, char* const argv []) {
    int child_pid;
    if ((child_pid = fork())) {
        int status;
        waitpid(child_pid, &status, WUNTRACED);
        return WEXITSTATUS(status);
    } else {
        return execvp(file, argv);
    }
}
