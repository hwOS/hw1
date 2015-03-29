#include <errno.h>

int error_read() {
    errno = EAGAIN;
    return -1;
}
