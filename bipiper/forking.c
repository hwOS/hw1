#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <bufio.h>
#define epr(...) fprintf(stderr, __VA_ARGS__);

#define EXPAND_STR(tok) #tok
#define TO_STR(arg) EXPAND_STR(arg)
#define cur_loc() __FILE__": line: "TO_STR(__LINE__)

#define check(arg) \
if (arg < 0) { \
    perror(cur_loc()); \
    exit(EXIT_FAILURE); \
}

#define write2(arg) write(STDERR_FILENO, arg, sizeof(arg));

const int BUF_SIZE = 4096;
const int BACKLOG = 50;

int get_listenning_socket(struct addrinfo* result) {
    struct addrinfo* cp;
    int listening_socket;
    for (cp = result; cp != NULL; cp = result->ai_next) {
        listening_socket = socket(AF_INET, SOCK_STREAM, 0); 
        if (listening_socket < 0) continue;

        int one = 1;
        if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == 0 && 
            bind(listening_socket, cp->ai_addr, cp->ai_addrlen) == 0) 
        {
            break;
        }

        close(listening_socket);
    }

    if (cp == NULL) {
        epr("can't bind socket on address: %s\n", result->ai_addr->sa_data);
        exit(EXIT_FAILURE);
    }

    check(listen(listening_socket, BACKLOG));
    return listening_socket;
}

void handler(int signum) {
    int status;
    check(wait(&status));
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        write2("child was been terminated abnormally\n Maybe SIGPIPE Oo\n");
    }
}

int do_fork() {
    int cpid = fork();
    if (cpid < 0) {
        perror(cur_loc());
        exit(EXIT_FAILURE);
    }
    return cpid;
}

void forward_messages(int first_client, int second_client) {
    if (do_fork() == 0) { // handler of first client
        buf_t* buf = buf_new(BUF_SIZE);
        ssize_t prev_size, res_fill; 
        do {
            prev_size = buf_size(buf);
            res_fill = buf_fill(first_client, buf, prev_size + 1); 
            check(res_fill);
            if (buf_size(buf) > 0)
                check(buf_flush(second_client, buf, 1));
        } while (res_fill > prev_size);
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stdout, "usage: ./forkint port1 port1");
        return 0;
    }
    struct sigaction sa;
    sa.sa_handler = handler;
    sigaction(SIGCHLD, &sa, NULL);
    const char* port1 = argv[1];
    const char* port2 = argv[2];

    struct addrinfo* result;    
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    check(getaddrinfo("localhost", port1, &hints, &result));
    int list_socket1 = get_listenning_socket(result);

    check(getaddrinfo(NULL, port2, &hints, &result));
    int list_socket2 = get_listenning_socket(result);

    while (1) {
        int first_client = accept(list_socket1, NULL, NULL);
        if (first_client < 0) {
            if (errno == EINTR) continue;

            perror(cur_loc());
            exit(EXIT_FAILURE);
        }

        int second_client;
        while (1) {
            second_client = accept(list_socket2, NULL, NULL);
            if (second_client < 0) {
                if (errno == EINTR) continue;

                perror(cur_loc());
                exit(EXIT_FAILURE);
            }
            break;
        }

        forward_messages(first_client, second_client);
        forward_messages(second_client, first_client);
        close(first_client);
        close(second_client);
    }

    return 0;
}
