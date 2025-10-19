#include "../include/ft_ping.h"

void xerror_die(const char *where, const char *why) {
    fprintf(stderr, "ft_ping: %s: %s\n", where, why);
    exit(EXIT_FAILURE);
}

double timeval_diff_ms(const struct timeval *end, const struct timeval *start) {
    long sec = end->tv_sec - start->tv_sec;
    long usec = end->tv_usec - start->tv_usec;

    return (double)sec * 1000.0 + (double)usec / 1000.0;
}

uint16_t icmp_checksum(const void *data, size_t len) {
    const uint16_t *w = (const uint16_t*)data;
    uint32_t        sum = 0;

    while (len > 1) {
        sum += *w++;
        len -= 2;
    }

    if (len) {
        sum += *(const u_int8_t*)w;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return (uint16_t)(~sum);
}