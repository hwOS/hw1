#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define ALLOC_SIZE 1024 * 1024 * 10

int main() {
    char *data;
    int i = 0;
    while (1) {
        data = (char*) malloc(ALLOC_SIZE);
        printf("%d\n", i++);
        if (data == NULL) {
            fprintf(stderr, "cannot allocate memory");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
        memset(data, 0xaa, ALLOC_SIZE);
    }
    return 0;
}
