#include "filter.h"
#include "helpers.h"
#include <stdio.h>
#define MAXSIZE 4097
#define MAX_CNT_ARGS 32769

char buf[MAXSIZE];

int main(int argc, char* argv[]) {
    int i;
    for (i = 1; i < argc; ++i) {
        argv[i - 1] = argv[i];
    }

    while (1) {
        if (!read_until(STDIN_FILENO, buf, MAXSIZE, '\n')) {
            break;
        }
        argv[argc - 1] = buf;
        argv[argc] = NULL;

        if (spawn(argv[0], argv) == 0) {
            printf("%s\n", buf);
        }
    }
    return 0;
}
