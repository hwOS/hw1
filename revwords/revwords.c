#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/helpers.h"
#define MAX_SIZE 4097

ssize_t read_bytes, i;
char buff[MAX_SIZE], temp;
char is_space;

/*
 * print first word from source and moves remaining characters to the begin
 * return result size of the source
 */
size_t print_word(char* source, char delimiter) {
    char* delimiter_pos = strchr(source, delimiter);
    size_t pos, i;
    size_t len = strlen(source);
    char temp;
    if (len == 0) return 0;

    if (delimiter_pos == NULL) {
        pos = len;
    } else {
        pos = (size_t)(delimiter_pos - source);
    }

    for (i = 0; i < pos / 2; ++i) {
        temp = source[i];
        source[i] = source[pos - i - 1];
        source[pos - i - 1] = temp;
    }

    if (delimiter_pos != NULL) {
        pos++;
    }
    write_(STDOUT_FILENO, source, pos);
    memmove(source, source + pos, len - pos + 1); 
    return len - pos + 1;
}

int main() {
    size_t read_bytes = 0;
    ssize_t last_read_bytes;
    while (1) {
        last_read_bytes = read_until(STDIN_FILENO, buff + read_bytes, MAX_SIZE, ' ');
        if (last_read_bytes == -1) {
            perror("in revwords: read_until: ");
            return EXIT_FAILURE;
        } else if (last_read_bytes == 0) {
            print_word(buff, ' ');
            return 0;
        }
        read_bytes += (size_t) last_read_bytes;
        buff[read_bytes] = 0;
        read_bytes = print_word(buff, ' ');
    }
    return 0;
}
