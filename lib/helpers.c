#define _GNU_SOURCE
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#define MAX_WORD_SIZE 4098

#define EXPAND_STR(tok) #tok
#define TO_STR(tok) EXPAND_STR(tok)
#ifdef DEBUG
#  define print_debug_info() perror(__FILE__": line "TO_STR(__LINE__))
#else
#  define print_debug_info() 
#endif

#define ret() return_state(); kill_childs(); print_debug_info(); return -1;

ssize_t read_(int fd, void *buf, size_t count) {
    size_t read_bytes = 0;
    ssize_t last_read_bytes = 0;
    char* buff = (char*) buf;
    while (read_bytes < count) {
        last_read_bytes = read(fd, buff + read_bytes, count - read_bytes);
        if (last_read_bytes == -1) {
            return -1;
        } else if (last_read_bytes == 0) {
            return (ssize_t) read_bytes;
        }
        read_bytes += (size_t) last_read_bytes;
    }
    return (ssize_t) read_bytes;
}

ssize_t write_(int fd, const void *buf, size_t count) {
    size_t wrote_bytes = 0;
    ssize_t last_wrote_bytes = 0;
    const char* buff = (const char*) buf;
    while (count > wrote_bytes) {
        last_wrote_bytes = write(fd, buff + wrote_bytes, count - wrote_bytes);
        if (last_wrote_bytes == -1) {
            return -1;
        }
        wrote_bytes += (size_t) last_wrote_bytes;
    }
    fsync(fd);
    return (ssize_t) wrote_bytes;
}

/*
 * return copy of the first word from the source 
 * and moves remaining characters (after delimiter) to the begin.
 * return NULL when delimiter was not found.
 */
char* get_word(char* source, char delimiter) {
    static char word[MAX_WORD_SIZE];
    size_t len = strlen(source);
    char* delimiter_pos = strchr(source, delimiter);
    size_t word_len = 0;

    if (delimiter_pos != NULL) {
        word_len = (size_t)(delimiter_pos - source);

        strncpy(word, source, word_len);
        word[word_len] = 0;
        if (len == word_len) {
            source[0] = 0;
        } else {
            memmove(source, source + word_len + 1, len - word_len); 
        }
        return word;
    }
    return NULL;
}

/*
 * finish work when delimiter or EOF met, or count byte was read.
 */
ssize_t read_until(int fd, void* buf, size_t count, char delimiter) {
    size_t i;
    ssize_t last_read_bytes = 0;
    size_t read_bytes = 0;
    char* buff = (char*) buf;

    while (read_bytes < count) {
        last_read_bytes = read(fd, buff + read_bytes, count - read_bytes);
        if (last_read_bytes == -1) {
            return -1;
        } else if (last_read_bytes == 0) {
            return (ssize_t) read_bytes;
        }

        read_bytes += (size_t) last_read_bytes;
        for (i = 0; i < (size_t) last_read_bytes; ++i) {
            if (buff[read_bytes - i - 1] == delimiter) {
                return (ssize_t) read_bytes;
            }
        }
    }
    return (ssize_t) read_bytes;
}

int spawn(const char * file, char* const argv []) {
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("helpers: spawn");
        return -1;
    }
    if (child_pid != 0) {
        int status;
        waitpid(child_pid, &status, WUNTRACED);
        return WEXITSTATUS(status);
    } else {
        execvp(file, argv);
        perror("helpers: spawn");
        _exit(EXIT_FAILURE);
    }
}

/*
 * construct execargs from string containing name of program and its arguments
 * string like: "./program arg1"
 * return NULL if program not found or malloc return NULL
 */
execargs_t* construct_execargs(char* start, char* end) {
    size_t count_args = 0;
    char* prev_start = start;
    
    while (1) {
        for (; start != end && *start == ' '; ++start);
        if (start == end) break;
        count_args++;    
        for (; start != end && *start != ' '; ++start);
    } 

    if (count_args == 0) {
        return NULL;
    }
    execargs_t* result = (execargs_t*) malloc(sizeof(execargs_t) + sizeof(char*) * (count_args + 1));
    if (result == NULL) {
        return NULL;
    }

    start = prev_start;
    for (int i = 0;; ++i) {
        for (;start < end && *start == ' '; ++start);
        if (start >= end) break;
        result->argv[i] = start;
        for (; start != end && *start != ' '; ++start);
        *(start++) = 0;
    } 
    result->argv[count_args] = NULL;
    result->name = result->argv[0];

    return result;
}

void destruct_execargs(execargs_t* execargs) {
    free(execargs);
}

int exec(execargs_t* args) {
    int cpid = fork();

    if (cpid < 0) {
        print_debug_info();
        return -1;
    }

    if (cpid == 0) { // child process
        /*fprintf(stderr, "\nfrom child: name=%s; argv[0]=%s; argv[1]=%s\n", args->name, args->argv[0], args->argv[1]);*/
        signal(SIGINT, SIG_DFL);
        if (execvp(args->name, args->argv) < 0) {
            print_debug_info();
            _exit(EXIT_FAILURE);
        }
    }

    /*fprintf(stderr, "child: %s; pid: %d\n", args->name, cpid);*/
    /*for (int i = 0; args->argv[i] != NULL; ++i) {*/
        /*fprintf(stderr, "argv[%d] = %s\n", i, args->argv[i]);*/
    /*}*/
    return cpid;
}

static int* childs_pid;
static size_t cnt_childs;
static size_t ran_childs;
static struct sigaction old_sigint;
static int old_stdin;
static int old_stdout;

static void kill_childs() {
    /*fprintf(stderr, "ran_child: %d\n", ran_childs);*/
    for (size_t i = 0; i < ran_childs; ++i) {
        // ignore errors occured during kill
        /*fprintf(stderr, "kill %d\n", (int) childs_pid[i]);*/
        kill(childs_pid[i], SIGKILL);
        waitpid(childs_pid[i], NULL, 0);
    }
}

void chld_handler(int signum) {
    signum = 10;
    /*fprintf(stderr, "signum = %d\n", signum);*/
    /*fprintf(stderr, "pid: %d\n", getpid());*/
    /*fprintf(stderr, "pid from: %d\n", info->si_pid);*/
    /*for (size_t i = 0; i < cnt_childs; ++i) {*/
        /*fprintf(stderr, "child_pid[%d] = %d\n", (int) i, childs_pid[i]);*/
    /*}*/
    /*fprintf(stderr, "in sigchld kill before\n");*/
    kill_childs();
    signal(SIGINT, SIG_IGN);
    /*fprintf(stderr, "in sigchld kill after\n");*/
}

static void return_state() {
    /*fprintf(stderr, "return state %d\n", getpid());*/
    sigaction(SIGINT, &old_sigint, 0);

    dup2(old_stdin, STDIN_FILENO);
    close(old_stdin);

    dup2(old_stdout, STDOUT_FILENO);
    close(old_stdout);
}

int runpiped(execargs_t** programs, const size_t n) 
{
    /*fprintf(stderr, "sigchld = %d\n", SIGCHLD);*/
    /*fprintf(stderr, "sigint = %d\n", SIGINT);*/
    struct sigaction sa;
    sa.sa_handler = chld_handler;
    sigemptyset(&sa.sa_mask);

    // error in sigaction never happened
    sigaction(SIGINT, &sa, &old_sigint);

    old_stdout = dup(STDOUT_FILENO);
    old_stdin = dup(STDIN_FILENO);

    int pipefd[2];
    int childs[n];
    childs_pid = childs;
    cnt_childs = n;
    ran_childs= 0;

    for (size_t i = 0; i < n - 1; ++i) {
        if (pipe2(pipefd, O_CLOEXEC) < 0) {
            ret();
        }

        int out_fd = pipefd[1];
        int in_fd = pipefd[0];

        
        if (dup2(out_fd, STDOUT_FILENO) < 0 || close(out_fd) < 0) {
            ret();
        }

        childs_pid[i] = exec(programs[i]);

        if (childs_pid[i] < 0 || dup2(in_fd, STDIN_FILENO) < 0 || close(in_fd) < 0) {
            ret();
        }
        ran_childs++;
    }

    // restore STDOUT descriptor to normal stdout for invoking the last program
    if (dup2(old_stdout, STDOUT_FILENO) < 0 || close(old_stdout) < 0) {
        ret();
    }

    childs_pid[n - 1] = exec(programs[n - 1]);

    if (childs_pid[n - 1] < 0 || close(STDIN_FILENO) < 0) {
        ret();
    }
    ran_childs++;

    for (size_t i = 0; i < n; ++i) {
        int status;
        wait(&status);
        /*fprintf(stderr, "terminated child %d\n", wait(&status));*/
        /*perror("term");*/
    }

    return_state();

    /*fprintf(stderr, "runpiped terminated\n");*/
    return 0;
}
