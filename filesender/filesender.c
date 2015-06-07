#include <stdio.h>
#include <bufio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <bufio.h>
#define epr(args...) fprintf(stderr, args);

#define EXPAND_STR(tok) #tok
#define TO_STR(tok) EXPAND_STR(tok)
#define cur_loc() __FILE__": line: "TO_STR(__LINE__)

#define check(arg) if (arg < 0) { \
                        perror(cur_loc()); \
                        exit(EXIT_FAILURE); \
                    }

const int BUF_SIZE = 4096;
const int BACKLOG = 50;

void handler(int signum) {
    int status;
    check(wait(&status));
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        epr("child was been terminated abnormally\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stdout, "usage: ./forkint port1 file_name");
        return 0;
    }
    struct sigaction sa;
    sa.sa_handler = handler;
    sigaction(SIGCHLD, &sa, NULL);

    const char* port = argv[1];
    const char* target_file = argv[2];
    int server_socket;

    struct addrinfo *result;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    check(getaddrinfo(NULL, port, &hints, &result));
    
    struct addrinfo* cp;
    for (cp = result; cp != NULL; cp = cp->ai_next) {
        server_socket = socket(cp->ai_family, cp->ai_socktype, cp->ai_protocol);
        if (server_socket < 0) continue;

        int one = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == 0 && 
            bind(server_socket, cp->ai_addr, cp->ai_addrlen) == 0) 
        {
            break;
        }

        close(server_socket);
    }

    if (cp == NULL) {
        perror(cur_loc());
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    check(listen(server_socket, BACKLOG));
    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            if (errno == EINTR) continue;

            perror(cur_loc());
            exit(EXIT_FAILURE);
        }

        int cpid = fork();
        check(cpid);
        if (cpid == 0) { // child process
            int target_fd = open(target_file, O_RDONLY);
            check(target_fd);
            
            buf_t* buf = buf_new(BUF_SIZE); 
            ssize_t res_fill, prev_size;
            do {
                prev_size = buf_size(buf);
                res_fill = buf_fill(target_fd, buf, 1);
                check(res_fill);
                if (buf_size(buf) > 0)
                    check(buf_flush(client_socket, buf, 1));
            } while (res_fill > prev_size);
            exit(EXIT_SUCCESS);
        }
        close(client_socket);
    }
    

    return 0;
}
