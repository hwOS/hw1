#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char buf[1000];
    int fl = open("file", O_RDONLY);
    if (fork()) {
        read(fl, buf, 5);
        printf("parent:%s\n", buf);
    } else {
        read(fl, buf, 5);
        printf("child:%s\n", buf);
    }
    return 0;
}
