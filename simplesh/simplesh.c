#include <helpers.h>
#include <bufio.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

/*execargs_t construct_execargs(const char* const name, char* const * argv);*/
/*int runpiped(execargs_t** programs, size_t n); */
/*int exec(execargs_t* args);*/
const int MAX_PIPELINE = 65536;
const int MAX_PROGRAMS = 65536;

int find_lf(buf_t* buf, size_t old_size) {
    for (size_t i = old_size; i < buf_size(buf); ++i) {
        if (buf->data[i] == '\n') {
            return i;
        }
    }
    return 0;
}

void handler(int signum) {
   fsync(STDOUT_FILENO);
   write(STDOUT_FILENO, "\n$ ", 3); 
}

int main() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) != 0) {
        perror("main");
        return -1;
    }
    

    buf_t *buf = buf_new(MAX_PIPELINE);
    execargs_t* programs[MAX_PROGRAMS];
    while (1) {
        fsync(STDOUT_FILENO);
        write(STDOUT_FILENO, "\n$ ", 3);
        size_t lf_pos = -1;
        buf_clear(buf);
        while (1) {
            size_t prev_size = buf_size(buf);
            if (buf_fill(STDIN_FILENO, buf, prev_size + 1) == 0) {
                return 0;
            }
            lf_pos = find_lf(buf, prev_size);
            if (lf_pos > 0) break;
        } 

        buf->data[lf_pos + 1] = 0;
        /*printf("str = %s; strlen = %d; lf_pos = %d\n", buf->data, strlen(buf->data), lf_pos);*/
        char* start = buf->data; 
        int cnt_programs = 0;
        while (start < buf->data + lf_pos) {
            char* end = strchr(start, '|');
            if (end == NULL) {
                end = buf->data + lf_pos;
            } 
            programs[cnt_programs++] = construct_execargs(start, end);
            start = end + 1;
        }

        /*for (int i = 0; i < cnt_programs; ++i) {*/
            /*printf("#%d: name: %s\n", i, programs[i]->name);*/
            /*for (int j = 0; programs[i]->argv[j] != NULL; ++j) {*/
                /*printf("--argv[%d]: %s, len = %d\n", j, programs[i]->argv[j], strlen(programs[i]->argv[j]));*/
            /*}*/
        /*}*/
        runpiped(programs, cnt_programs);
    }
    return 0;
}
