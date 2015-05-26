#include <helpers.h>
#include <stdio.h>

int main() {
    char* const argv[] = {"ls"};
    char* const argv2[] = {"grep", ".c"};

    execargs_t program1 = construct_execargs("ls", argv);
    execargs_t program2 = construct_execargs("grep", argv2);
    execargs_t* programs[2] = {
        &program1,
        &program2
    };

    runpiped(programs, 2); 
    return 0;
}
