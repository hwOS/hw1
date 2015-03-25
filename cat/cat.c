#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#define MAX_SIZE 4097

char buff[MAX_SIZE];
ssize_t read_bytes, wrote_bytes;

int main() {
    while (1) {
        read_bytes = read_(STDIN_FILENO, buff, MAX_SIZE);
        if (read_bytes == -1) {
            perror("in cat: read_: ");
            return EXIT_FAILURE;
        } else if (read_bytes == 0) {
            return 0;
        }

        wrote_bytes = write_(STDOUT_FILENO, buff, (size_t) read_bytes);
        if (wrote_bytes == -1) {
            perror("in cat: write_: ");
            return EXIT_FAILURE;
        }
    }
    return 0;
}
