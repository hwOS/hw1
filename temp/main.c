#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char s[1000];
    char buf[1000];
    scanf("%s", s);
    printf("%s\n", s);
    /*scanf("%s", s);*/
    /*printf("%s\n", s);*/
    read(STDIN_FILENO, buf, 5);
    printf("buf: %s\n", buf);
    return 0;
}
