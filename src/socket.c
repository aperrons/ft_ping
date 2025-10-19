#include "../include/ft_ping.h"

int open_raw_socket(void) {
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (fd < 0) {
        xerror_die("socket", "eed root or CAP_NET_RAW");
        return fd;
    }

    return fd;
}

void set_socket_timeout(int fd, int timeout_ms) {
    struct timeval timeval;
    
    timeval.tv_sec = timeout_ms / 1000;
    timeval.tv_usec = (timeout_ms % 1000) * 1000;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeval, sizeof(timeval)) < 0) {
        perror("setsockopt(SO_RCVTIMEO)");
    }
}

ssize_t recv_packet(int fd, void *buffer, size_t buffer_len, struct sockaddr_in *from, struct timeval *timeval) {
    struct iovec    iov;
    struct msghdr   msg;
    ssize_t         n;

    iov.iov_base = buffer;
    iov.iov_len = buffer_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = from;
    msg.msg_namelen = sizeof(*from);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    n = recvmsg(fd, &msg, 0);
    if (n > 0 && timeval) {
        gettimeofday(timeval, NULL);
    }

    return n;
}