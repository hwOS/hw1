#include "filter.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE 4097
#define MAX_CNT_ARGS 32769
#define STR_DEL '\n'

char buf[MAXSIZE];

void run_spawn(int argc, char** argv, char* last_arg) {
    size_t len_last_arg = strlen(last_arg);
    argv[argc - 1] = last_arg;
    argv[argc] = NULL;

    if (spawn(argv[0], argv) == EXIT_SUCCESS) {
        last_arg[len_last_arg++] = STR_DEL;
        write_(STDOUT_FILENO, last_arg, len_last_arg);
    }
}

int main(int argc, char* argv[]) {
    int i;
    char* last_arg;
    ssize_t res_read;
    for (i = 1; i < argc; ++i) {
        argv[i - 1] = argv[i];
    }

    while (1) {
        res_read = read_until(STDIN_FILENO, buf, MAXSIZE, '\n');
        if (res_read == -1) {
            perror("filter: read_until");
            return EXIT_FAILURE;
        } else if (res_read == 0) {
            if (buf[0] != 0) {
                run_spawn(argc, argv, buf);
            }
            return 0;
        }

        while ((last_arg = get_word(buf, '\n'))) {
            run_spawn(argc, argv, last_arg);
        }
    }
    return 0;
}
