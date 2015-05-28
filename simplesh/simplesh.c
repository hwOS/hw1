#include <helpers.h>
#include <stdio.h>
#include <sys/wait.h>

int main() {
    char* const argv1[] = {"find", "/", NULL};
    char* const argv2[] = {"grep", "\\.c", NULL};
    char* const argv3[] = {"head", "-n 10000", NULL};

    execargs_t program1 = construct_execargs("find", argv1);
    execargs_t program2 = construct_execargs("grep", argv2);
    execargs_t program3 = construct_execargs("head", argv3);

    execargs_t* programs[3] = {
        &program1,
        &program2,
        &program3
    };

    runpiped(programs, 3); 
    fprintf(stderr, "after runpiped");

    int x;
    scanf("%d", &x);
    printf("x = %d", x);
    return 0;
}
