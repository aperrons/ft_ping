#include "../include/ft_ping.h"

void print_ping_header(const t_target *target) {
    printf("PING %s (%s) %d(%d) bytes of data.\n",
           target->fqdn, target->ip_str, FT_PING_PAYLOAD_SIZE,
           (int)(FT_PING_PAYLOAD_SIZE + 8 + 20));
    fflush(stdout);
}

void print_reply_line(const char *from_ip, size_t bytes, uint16_t seq, uint8_t ttl, double rtt_ms) {
    printf("%zu bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n",
           bytes, from_ip, seq, ttl, rtt_ms);
}

void print_verbose_icmp_notice(const char *msg) {
    fprintf(stderr, "ft_ping: %s\n", msg);
}

void print_timeout_line(const char *from_ip, uint16_t seq, bool verbose) {
    (void)from_ip;
    (void)verbose;
    printf("Request timeout for icmp_seq=%u\n", seq);
    fflush(stdout);
}