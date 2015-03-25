#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/helpers.h"
#define MAX_SIZE 7

ssize_t read_bytes;
char buff[MAX_SIZE];
size_t cur_pos, len, res_len, i, num_bytes;
char flag;
char* word;

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("please enter word");
        return 0;
    }
    
    word = argv[1];
    len = strlen(word);
    len = len == 0 ? 1 : len;
    while (1) {
        if (num_bytes > 0) {
            read_bytes = read_(STDIN_FILENO, buff + num_bytes, MAX_SIZE - num_bytes);
        } else {
            read_bytes = read_(STDIN_FILENO, buff, MAX_SIZE);
        }

        if (read_bytes == -1) {
            perror("in delwords: read_: ");
            return EXIT_FAILURE;
        } else if (read_bytes == 0) {
            write_(STDOUT_FILENO, buff, num_bytes);
            return 0;
        }
        num_bytes += (size_t) read_bytes;
        res_len = 0;
        cur_pos = 0;

        if (num_bytes >= len) {
            while(cur_pos <= num_bytes - len) {
                flag = 1;
                for (i = 0; cur_pos + i < num_bytes && i < len; i++) {
                    if (word[i] != buff[cur_pos + i]) {
                        flag = 0;
                        break;
                    }
                }
                if (flag == 1) {
                    cur_pos += len;
                } else {
                    buff[res_len] = buff[cur_pos];
                    cur_pos++;
                    res_len++;
                }
            }

            write_(STDOUT_FILENO, buff, res_len);
            num_bytes -= cur_pos;
            for (i = 0; i < num_bytes; i++) {
                buff[i] = buff[cur_pos + i]; 
            }
            buff[num_bytes] = 0;
        }
    }
    return 0;
}
