#ifndef HELPERS_H
#define HELPERS_H
#include <unistd.h>

ssize_t read_(int fd, void* buf, size_t count);
ssize_t write_(int fd, const void* buf, size_t count);
ssize_t read_until(int fd, void* buf, size_t count, char delimiter);
int spawn(const char * file, char * const argv []);

/*
 * return copy of the first word from the source 
 * and moves remaining characters (after delimiter) to the begin.
 * return NULL when delimiter was not found.
 */
char* get_word(char* source, char delimiter);

typedef struct {
    const char* name;
    const char* argv[];
} execargs_t;

execargs_t* construct_execargs(const char* name, const char* argv[]);
int exec(execargs_t* args);

#endif
