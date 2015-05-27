#include <helpers.h>
#include <stdio.h>
#include <sys/wait.h>

int main() {
    char* const argv[] =  {"cat", NULL};
    char* const argv2[] = {"ls", NULL};
    char* const argv3[] = {"less", NULL};

    execargs_t program1 = construct_execargs("cat", argv);
    execargs_t program2 = construct_execargs("ls", argv2);
    execargs_t program3 = construct_execargs("less", argv3);
    execargs_t* programs[3] = {
        &program1,
        &program2,
        &program3
    };

    runpiped(programs, 3); 

    int x;
    scanf("%d", &x);
    printf("x = %d", x);
    return 0;
}
