#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int error_read();

int main() {
    if (error_read() == -1) {
        perror("error occured");
        return EXIT_FAILURE;
    }
    return 0;
}
