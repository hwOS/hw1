#include <bufio.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_BUF_LEN 4096

int main() {
    buf_t *buf = buf_new(MAX_BUF_LEN);
#ifdef DEBUG
    assert(buf != NULL);
#endif
    ssize_t res_fill = 1;
    while (res_fill) {
        res_fill = buf_fill(STDIN_FILENO, buf, buf->capacity - buf->size);
        if (buf_flush(STDOUT_FILENO, buf, buf->size) == -1 || res_fill == -1) {
            exit(EXIT_FAILURE);
        }
    }
    
    return 0;
}
