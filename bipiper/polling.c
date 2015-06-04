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
#define BUF_SIZE 4096
#define BUF_ID (id / 2 - 1)
#define LAST_BUF (cnt_fds / 2 - 2)
#define BACKLOG 500

#define check(cond) \
if (cond != 0) { \
    perror(cur_loc()); \
    exit(EXIT_FAILURE); \
}

int get_listenning_socket(char* port) {
    struct addrinfo* result;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    check(getaddrinfo("localhost", port, &hints, &result));

    int res_socket;
    struct addrinfo* cp = NULL;
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

        if (listen(res_socket, BACKLOG) < 0) {
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

void swap_buff(buf_t** buff1, buf_t** buff2) {
    buf_t* temp = *buff1;
    *buff1 = *buff2;
    *buff2 = temp;
}

void swap_buffs(buf_t** buff1, buf_t** buff2) {
    swap_buff(buff1, buff2);
    swap_buff(buff1 + 1, buff2 + 1);
}

void swap_pollfd(struct pollfd* a, struct pollfd* b) {
    struct pollfd temp = *a;
    *a = *b;
    *b = temp;
}


struct pollfd fds[MAX_CLIENTS];
buf_t* buffs[MAX_CLIENTS / 2 - 1][2];
size_t cnt_fds = 2;

void close_and_swap(int id, int id2) {
    if (cnt_fds == MAX_CLIENTS) {
        if (MAX_CLIENTS % 2 == 0) {
            fds[0].events = POLLIN;
        } else {
            fds[1].events = POLLIN;
        }
    }
    int min_id = id < id2 ? id : id2;
    int max_id = id > id2 ? id : id2;
    close(fds[min_id].fd);
    close(fds[max_id].fd);
    fds[min_id].revents = 0;
    fds[max_id].revents = 0;
    swap_pollfd(&fds[min_id], &fds[cnt_fds - 2]);
    swap_pollfd(&fds[max_id], &fds[cnt_fds - 1]);
    cnt_fds -= 2;
}

void event_on_accept_socket(int id) {
    fds[id].events = 0;
    fds[cnt_fds].fd = accept(fds[id].fd, NULL, NULL);
    fds[cnt_fds].events = 0;
    if (fds[cnt_fds].fd < 0) {
        perror(cur_loc());
        return ;
    }
    cnt_fds++;
    if (cnt_fds != MAX_CLIENTS)
        fds[id ^ 1].events = POLLIN;

    if (cnt_fds % 2 == 0) {
        fds[cnt_fds - 2].events = POLLIN;
        fds[cnt_fds - 1].events = POLLIN;
        if (buffs[LAST_BUF][0] == NULL) {
            buffs[LAST_BUF][0] = buf_new(BUF_SIZE); 
            buffs[LAST_BUF][1] = buf_new(BUF_SIZE); 
        } else {
            buf_clear(buffs[LAST_BUF][0]);
            buf_clear(buffs[LAST_BUF][1]);
        }
    }
}

void sock_events(size_t *i) {
    size_t id = *i;
    if (id < 2) {
        event_on_accept_socket(id);
        return;
    }
    buf_t* r_buf = buffs[BUF_ID][id & 1];
    buf_t* w_buf = buffs[BUF_ID][!(id & 1)];
    int id2 = id % 2 == 0 ? id + 1 : id - 1; 

    if (fds[id].revents & POLLIN) {

        size_t prev_size = buf_size(r_buf);
        ssize_t res_fill = buf_fill(fds[id].fd, r_buf, prev_size + 1);
        if (res_fill < 0 || (size_t) res_fill == prev_size) {
            *i -= 1;
            swap_buffs(buffs[BUF_ID], buffs[LAST_BUF]);
            close_and_swap(id, id2);
            return;
        }

        if (buf_size(r_buf) == r_buf->capacity) {
            fds[id].events ^= POLLIN;
        }

        fds[id2].events |= POLLOUT;
    } else { // POLLOUT
        if (buf_flush(fds[id].fd, w_buf, 1) < 0) {
            *i -= 1;
            swap_buffs(buffs[BUF_ID], buffs[LAST_BUF]);
            close_and_swap(id, id2);
            return;
        }

        if (buf_size(w_buf) == 0) {
            fds[id].events ^= POLLOUT;
        }

        fds[id2].events |= POLLIN;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("usage: ./polling port1 port2");
        return 0;
    }
    memset(fds, 0, sizeof(fds));
    
    fds[0].fd = get_listenning_socket(argv[1]);
    fds[0].events = POLLIN;
    fds[1].fd = get_listenning_socket(argv[2]);

    while (1) {
        int id = poll(fds, cnt_fds, -1);
        if (id < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror(cur_loc());
            exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < cnt_fds; ++i) {
            if (fds[i].revents != 0) {
                sock_events(&i);
            }
        }
    }
    
    return 0;
}
