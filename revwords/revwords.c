#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/helpers.h"
#define MAX_SIZE 4097
ssize_t read_bytes, i;
char buff[MAX_SIZE];
char is_space;

/* source - null-terminated string */
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

#define MAX_WORD_SIZE 4098
struct word {
    char* data;
    ssize_t len;
};

/*
 * return struct word that contains copy of first word from the source 
 * and moves remaining characters (after delimiter) to the begin.
 * field len = -1 when delimiter was not found.
 */
struct word get_word(char* source, char delimiter) {
    static char data[MAX_WORD_SIZE];
    static struct word res;
    size_t len = strlen(source);
    char* delimiter_pos = strchr(source, delimiter);
    res.data = data;
    res.len = -1;

    if (delimiter_pos != NULL) {
        res.len = (ssize_t)(delimiter_pos - source);
        res.data[res.len] = 0;

        strncpy(data, source, (size_t) res.len);
        if (len == (size_t) res.len) {
            source[0] = 0;
        } else {
            memmove(source, source + res.len + 1, len - (size_t) res.len); 
        }
    }
    return res;
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
    struct word first_word = get_word(source, delimiter);
    if (first_word.len == -1) {
        return -1;
    }
    reverse_word(first_word.data);
    first_word.data[first_word.len] = delimiter;

    if ((size_t) first_word.len != len) {
        first_word.data[first_word.len] = delimiter;
        first_word.len++;
    }

    write_(STDOUT_FILENO, first_word.data, (size_t) first_word.len);
    return first_word.len;
}

int main() {
    size_t read_bytes = 0;
    ssize_t last_read_bytes;
    ssize_t res_pr_w;
    while (1) {
        last_read_bytes = read_until(STDIN_FILENO, buff + read_bytes, MAX_SIZE, ' ');
        if (last_read_bytes == -1) {
            perror("in revwords: read_until");
            return EXIT_FAILURE;
        } else if (last_read_bytes == 0) {
            print_word(buff, '\0');
            return 0;
        }
        read_bytes += (size_t) last_read_bytes;
        while (1) {
            buff[read_bytes] = 0;
            if ((res_pr_w = print_word(buff, ' ')) == -1) {
                break;
            }
            read_bytes -= (size_t) res_pr_w;
        }
    }
    return 0;
}
