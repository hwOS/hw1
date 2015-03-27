#include "filter.h"
#include "helpers.h"
#include <stdio.h>
#define MAXSIZE 4097
#define MAX_CNT_ARGS 32769

char buf[MAXSIZE];
char* argv2[MAX_CNT_ARGS];

int main(int argc, char* argv[]) {
    int i;
    for (i = 1; i < argc; ++i) {
        argv2[i - 1] = argv[i];
    }

    argc--; 
    while (1) {
        if (!read_until(STDIN_FILENO, buf, MAXSIZE, '\n')) {
            break;
        }
        argv2[argc] = buf;
        argv2[argc + 1] = NULL;

        if (spawn(argv2[0], argv2) == 0) {
            printf("%s\n", buf);
        }
    }
    return 0;
}
