#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <bufio.h>
#include <string.h>
#include <errno.h>

#define epr(...) fprintf(stderr, __VA_ARGS__);
#define EXP_STR(tok) #tok
#define TO_STR(arg) EXP_STR(arg)
#define cur_loc() __FILE__": line: "TO_STR(__LINE__)
#define MAX_CLIENTS 256

#define check(cond) \
if (cond != 0) { \
    perror(cur_loc()); \
    exit(EXIT_FAILURE); \
}

typedef struct {
    buf_t* buffer1;
    buf_t* buffer2;
} buff;

int get_listenning_socket(char* port) {
    struct addrinfo* result;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    check(getaddrinfo("localhost", port, &hints, &result));

    int res_socket;
    struct addrinfo* cp;
    for (cp = result; cp != NULL; ++cp) {
        res_socket = socket(cp->ai_family, cp->ai_socktype, cp->ai_protocol); 
        if (res_socket < 0) 
            continue;
        int one = 1;
        if (setsockopt(res_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) {
            close(res_socket);
            continue;
        }

        if (bind(res_socket, cp->ai_addr, cp->ai_addrlen) < 0) {
            close(res_socket);
            continue;
        }

        if (listen(res_socket, MAX_CLIENTS) < 0) {
            close(res_socket);
            continue;
        }
        break;
    }

    if (cp == NULL) {
        epr("can't bind on port: %s\n", port);
        exit(EXIT_FAILURE);
    }

    return res_socket;
}

int main(int argc, char* argv[]) {
    /*if (argc < 3) {*/
        /*printf("usage: ./polling port1 port2");*/
        /*return 0;*/
    /*}*/
    struct pollfd fds[MAX_CLIENTS];
    memset(fds, 0, sizeof(fds));
    buff buffs[MAX_CLIENTS / 2 - 1];
    size_t cnt_clients = 2;
    
    fds[0].fd = get_listenning_socket(argv[1]);
    fds[0].events = POLLIN;
    fds[1].fd = get_listenning_socket(argv[2]);

    while (1) {
        int id = poll(fds, cnt_clients, -1);
        if (id < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror(cur_loc());
            exit(EXIT_FAILURE);
        }

        if (id < 2) { // event occured on accept sockets
            fds[id].events = 0;
            fds[cnt_clients].fd = accept(fds[id].fd, NULL, NULL);
            cnt_clients++;
            if (cnt_clients != MAX_CLIENTS)
                fds[id ^ 1].events = POLLIN;
        } else {

        }
    }
    

    return 0;
}
