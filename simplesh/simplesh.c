#include <helpers.h>
#include <bufio.h>
#include <stdio.h>
#include <sys/wait.h>


/*execargs_t construct_execargs(const char* const name, char* const * argv);*/
/*int runpiped(execargs_t** programs, size_t n); */
/*int exec(execargs_t* args);*/

void test() {
    char str[] = "find / | grep \\.c | head -n 100";
    execargs_t* program1 = construct_execargs(str, str + 8);
    execargs_t* program2 = construct_execargs(str + 9, str + 20);
    execargs_t* program3 = construct_execargs(str + 21, str + 33);

    execargs_t* programs[3] = {
        program1,
        program2,
        program3
    };

    runpiped(programs, 3); 
    fprintf(stderr, "after runpiped");

    int x;
    scanf("%d", &x);
    printf("x = %d", x);
}

const int MAX_PIPELINE = 65536;

int main() {
    test();
    return 0;
    buf_t *buf = buf_new(MAX_PIPELINE);
    int i = 0;
    while (1) {
        write(STDOUT_FILENO, "$ ", 2);
        buf_fill(STDIN_FILENO, buf, 1);
        for (size_t i = 0; i < buf_size(buf); ++i) {
            if (buf->data[i] == '\n') {
                buf->data[i] = '@';
            } 
        }
        if (i++ == 0)
            sleep(2);
        buf_flush(STDOUT_FILENO, buf, buf_size(buf));
    }
    return 0;
}
