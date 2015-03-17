#include <stdio.h>
#include <stdlib.h>
#include "../lib/helpers.h"
#define MAX_SIZE 4096

ssize_t read_bytes, i;
char buff[MAX_SIZE], temp;
char is_space;

int main() {
    while (1) {
        read_bytes = read_until(STDIN_FILENO, buff, MAX_SIZE, ' ');
        if (read_bytes == -1) {
            perror("in revwords: read_until: ");
            return EXIT_FAILURE;
        } else if (read_bytes == 0) {
            return 0;
        }

        is_space = 0;
        if (buff[read_bytes - 1] == ' ') {
            is_space = 1;
        }
        for (i = 0; i < read_bytes / 2; i++) {
            temp = buff[i];
            buff[i] = buff[read_bytes - i - 1 - is_space];
            buff[read_bytes - i - 1 - is_space] = temp;
        }
        write_(STDOUT_FILENO, buff, (size_t) read_bytes);
    }
    return 0;
}
