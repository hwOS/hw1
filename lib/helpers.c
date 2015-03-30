#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#define MAX_WORD_SIZE 4098

ssize_t read_(int fd, void *buf, size_t count) {
    size_t read_bytes = 0;
    ssize_t last_read_bytes = 0;
    char* buff = buf;
    while (read_bytes < count) {
        last_read_bytes = read(fd, buff + read_bytes, count - read_bytes);
        if (last_read_bytes == -1) {
            return -1;
        } else if (last_read_bytes == 0) {
            return (ssize_t) read_bytes;
        }
        read_bytes += (size_t) last_read_bytes;
    }
    return (ssize_t) read_bytes;
}

ssize_t write_(int fd, const void *buf, size_t count) {
    size_t wrote_bytes = 0;
    ssize_t last_wrote_bytes = 0;
    const char* buff = buf;
    while (count > wrote_bytes) {
        last_wrote_bytes = write(fd, buff + wrote_bytes, count - wrote_bytes);
        if (last_wrote_bytes == -1) {
            return -1;
        }
        wrote_bytes += (size_t) last_wrote_bytes;
    }
    fsync(fd);
    return (ssize_t) wrote_bytes;
}

/*
 * return copy of the first word from the source 
 * and moves remaining characters (after delimiter) to the begin.
 * return NULL when delimiter was not found.
 */
char* get_word(char* source, char delimiter) {
    static char word[MAX_WORD_SIZE];
    size_t len = strlen(source);
    char* delimiter_pos = strchr(source, delimiter);
    size_t word_len = 0;

    if (delimiter_pos != NULL) {
        word_len = (size_t)(delimiter_pos - source);

        strncpy(word, source, word_len);
        word[word_len] = 0;
        if (len == word_len) {
            source[0] = 0;
        } else {
            memmove(source, source + word_len + 1, len - word_len); 
        }
        return word;
    }
    return NULL;
}

/*
 * finish work when delimiter or EOF met, or count byte was read.
 */
ssize_t read_until(int fd, void* buf, size_t count, char delimiter) {
    size_t i;
    ssize_t last_read_bytes = 0;
    size_t read_bytes = 0;
    char* buff = buf;

    while (read_bytes < count) {
        last_read_bytes = read(fd, buff + read_bytes, count - read_bytes);
        if (last_read_bytes == -1) {
            return -1;
        } else if (last_read_bytes == 0) {
            return (ssize_t) read_bytes;
        }

        read_bytes += (size_t) last_read_bytes;
        for (i = 0; i < (size_t) last_read_bytes; ++i) {
            if (buff[read_bytes - i - 1] == delimiter) {
                return (ssize_t) read_bytes;
            }
        }
    }
    return (ssize_t) read_bytes;
}

int spawn(const char * file, char* const argv []) {
    int child_pid;
    if ((child_pid = fork())) {
        int status;
        waitpid(child_pid, &status, WUNTRACED);
        return WEXITSTATUS(status);
    } else {
        int dev_null = open("/dev/null", O_RDONLY);
        if (fcntl(dev_null, F_SETFD, FD_CLOEXEC) == -1) {
            perror("helpers:spawn:fcntl");
            exit(1);
        }
        while ((dup2(dev_null, STDERR_FILENO) == -1) && (errno == EINTR)); 

        return execvp(file, argv);
    }
}
