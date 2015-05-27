#include <helpers.h>
#include <stdio.h>
#include <sys/wait.h>

int main() {
    char* const argv1[] = {"find", "/", NULL};
    char* const argv2[] = {"head", NULL};
    char* const argv3[] = {"sleep", "3", NULL};
    char* const argv4[] = {"head", NULL};
    char* const argv5[] = {"sleep", "5", NULL};

    execargs_t program1 = construct_execargs("find", argv1);
    execargs_t program2 = construct_execargs("head", argv2);
    execargs_t program3 = construct_execargs("sleep", argv3);
    execargs_t program4 = construct_execargs("head", argv4);
    execargs_t program5 = construct_execargs("sleep", argv5);

    execargs_t* programs[5] = {
        &program1,
        &program2,
        &program3,
        &program4,
        &program5
    };

    runpiped(programs, 5); 
    fprintf(stderr, "after runpiped");

    int x;
    scanf("%d", &x);
    printf("x = %d", x);
    return 0;
}
