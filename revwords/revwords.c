#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/helpers.h"
#define MAX_SIZE 4098

/* source has to null-terminated string */
void reverse_word(char* source) {
    size_t cnt = strlen(source);
    size_t i;
    char temp;
    for (i = 0; i < cnt / 2; ++i) {
        temp = source[i];
        source[i] = source[cnt - i - 1];
        source[cnt - i - 1] = temp;
    }
}

/*
 * print first word, in reverse order with delimiter(except len(word) == len(source)), from source. 
 * moves remaining characters to the begin
 * return:
 *   if len(word) != len(source) return len(word) + 1
 *   if len(word) == len(source) return len(source)
 *   if delimiter was not found  return -1
 */
ssize_t print_word(char* source, char delimiter) {
    size_t len = strlen(source);
    char* word = get_word(source, delimiter);
    size_t word_len;
    if (word == NULL) {
        return -1;
    }
    word_len = strlen(word);
    reverse_word(word);
    word[word_len] = delimiter;

    if (word_len != len) {
        word[word_len] = delimiter;
        word_len++;
    }

    write_(STDOUT_FILENO, word, word_len);
    return (ssize_t) word_len;
}

int main() {
    char buf[MAX_SIZE];
    size_t read_bytes = 0;
    ssize_t last_read_bytes;
    ssize_t res_pr_w;
    while (1) {
        last_read_bytes = read_until(STDIN_FILENO, buf + read_bytes, MAX_SIZE, ' ');
        if (last_read_bytes == -1) {
            perror("in revwords: read_until");
            return EXIT_FAILURE;
        } else if (last_read_bytes == 0) {
            print_word(buf, '\0');
            return 0;
        }
        read_bytes += (size_t) last_read_bytes;
        while (1) {
            buf[read_bytes] = 0;
            if ((res_pr_w = print_word(buf, ' ')) == -1) {
                break;
            }
            read_bytes -= (size_t) res_pr_w;
        }
    }
    return 0;
}
