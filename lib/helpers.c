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
#define MAX_WORD_SIZE 4098

#define EXPAND_STR(tok) #tok
#define TO_STR(tok) EXPAND_STR(tok)
#define print_debug_info() perror(__FILE__": line "TO_STR(__LINE__))

#ifdef DEBUG
#  define print_debug_and_ret() print_debug_info(); return -1;
#else
#  define print_debug_and_ret() return -1;
#endif

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


execargs_t construct_execargs(const char* const name, char* const * argv) {
    execargs_t result = {name, argv};
    return result;
}

int exec(execargs_t* args) {
    int cpid = fork();

    if (cpid < 0) {
        print_debug_and_ret();
    }

    if (cpid == 0) { // child process
        /*fprintf(stderr, "set default sigint for: %d\n", getpid());*/

        signal(SIGINT, SIG_DFL);
        if (execvp(args->name, args->argv) < 0) {
            print_debug_and_ret();
        }
    }
    return cpid;
}

static int* childs_pid;
static size_t cur_child = 0;
static size_t cnt_childs;
static int child_status;

void chld_handler(int signum, siginfo_t* info, void* ignore) {
    /*fprintf(stderr, "signum = %d\n", signum);*/
    /*fprintf(stderr, "pid: %d\n", getpid());*/
    /*fprintf(stderr, "pid from: %d\n", info->si_pid);*/
    /*for (size_t i = 0; i < cnt_childs; ++i) {*/
        /*fprintf(stderr, "child_pid[%d] = %d\n", (int) i, childs_pid[i]);*/
    /*}*/
    if (signum == SIGCHLD && info->si_pid == childs_pid[cur_child]) {
        /*fprintf(stderr, "in sigchld before\n");*/
        waitpid(info->si_pid, &child_status, 0);
        child_status = !WIFEXITED(child_status);
        cur_child++;
        /*fprintf(stderr, "in sigchld after\n");*/
        return;
    } else { // SIGCHLD or SIGINT
        /*fprintf(stderr, "in sigchld kill before\n");*/
        for (size_t i = cur_child; i < cnt_childs; ++i) {
            /*fprintf(stderr, "kill %d\n", (int) childs_pid[i]);*/
            if (kill(childs_pid[i], SIGINT) < 0) {
                print_debug_info();
                return;
            }
        }
        cur_child = cnt_childs;
        signal(SIGCHLD, SIG_IGN);
        /*fprintf(stderr, "in sigchld kill after\n");*/
    }
}

int runpiped(execargs_t** programs, const size_t n) {
    /*fprintf(stderr, "sigchld = %d\n", SIGCHLD);*/
    /*fprintf(stderr, "sigint = %d\n", SIGINT);*/
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = chld_handler;

    /* Signals blocked during the execution of the handler. */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGCHLD);

    if (sigaction(SIGCHLD, &sa, 0) != 0 || sigaction(SIGINT, &sa, 0) != 0) {
        print_debug_and_ret();
    }

    int old_stdout = dup(STDOUT_FILENO);
    int old_stdin = dup(STDIN_FILENO);

    int pipefd[2 * (n - 1)];
    int childs[n];
    childs_pid = childs;
    cnt_childs = n;
    child_status = 0;

    for (size_t i = 0; i < n - 1; ++i) {
        if (pipe(pipefd + 2 * i) < 0) {
            print_debug_and_ret();
        }

        int out_fd = pipefd[2 * i + 1];
        if (fcntl(out_fd, F_SETFD, FD_CLOEXEC) == -1) {
            print_debug_and_ret();
        }

        int in_fd = pipefd[2 * i];
        if (dup2(out_fd, STDOUT_FILENO) < 0) {
            print_debug_and_ret();
        }

        childs[i] = exec(programs[i]);
        /*fprintf(stderr, "in_fd = %d; out_fd = %d; childs[i] = %d\n", in_fd, out_fd, childs[i]);*/
        if (childs[i] < 0) {
            print_debug_and_ret();
        }

        if (dup2(in_fd, STDIN_FILENO) < 0) {
            print_debug_and_ret();
        }
    }

    if (dup2(old_stdout, STDOUT_FILENO) < 0 || close(old_stdout) < 0) {
        print_debug_and_ret();
    }

    childs[n - 1] = exec(programs[n - 1]);

    if (childs[n - 1] < 0 || dup2(old_stdin, STDIN_FILENO) < 0 || close(old_stdin) < 0) {
        print_debug_and_ret();
    }

    /*fprintf(stderr, "before for\n");*/
    for (size_t i = 0; i < n; ++i) {
        /*fprintf(stderr, "out_fd = %d; in_fd = %d; childs[i] = %d\n", pipefd[2 * i + 1], pipefd[2 * i], childs[i]);*/
        waitpid(childs[i], NULL, 0);

        if ((i < n - 1 && close(pipefd[2 * i + 1]) < 0) || 
           (i > 0 && close(pipefd[2 * i - 2]) < 0 ) || child_status != 0) 
        {
            print_debug_and_ret();
        }
    }
    /*fprintf(stderr, "runpiped terminated\n");*/

    return 0;
}
