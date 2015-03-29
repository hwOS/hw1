#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/helpers.h"
#define MAX_SIZE 4097

ssize_t read_bytes, i;
char buff[MAX_SIZE];
char is_space;

void reverse_word(char* source, size_t cnt) {
    size_t i;
    char temp;
    for (i = 0; i < cnt / 2; ++i) {
        temp = source[i];
        source[i] = source[cnt - i - 1];
        source[cnt - i - 1] = temp;
    }
}

/*
 * print first word, in reverse order, from source and moves remaining characters to the begin
 * return result size of the source
 */
ssize_t print_word(char* source, char delimiter) {
    size_t len = strlen(source);
    char* delimiter_pos = strchr(source, delimiter);
    size_t pos;
    if (delimiter_pos == NULL) {
        return -1;
    } 
    pos = (size_t)(delimiter_pos - source);
    reverse_word(source, pos);

    if (pos != len) {
        pos++;
    }
    write_(STDOUT_FILENO, source, pos);
    memmove(source, source + pos, len - pos + 1); 
    return (ssize_t) (len - pos + 1);
}

int main() {
    size_t read_bytes = 0;
    ssize_t last_read_bytes;
    ssize_t res_pr_w;
    while (1) {
        last_read_bytes = read_until(STDIN_FILENO, buff + read_bytes, MAX_SIZE, ' ');
        if (last_read_bytes == -1) {
            perror("in revwords: read_until: ");
            return EXIT_FAILURE;
        } else if (last_read_bytes == 0) {
            print_word(buff, '\0');
            return 0;
        }
        while (1) {
            read_bytes += (size_t) last_read_bytes;
            buff[read_bytes] = 0;
            if ((res_pr_w = print_word(buff, ' ')) == -1) {
                break;
            }
            read_bytes = (size_t) res_pr_w;
        }
    }
    return 0;
}
