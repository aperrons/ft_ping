#include "../include/ft_ping.h"

size_t build_echo_request(uint8_t *buffer, size_t buffer_len, uint16_t ident, uint16_t seq, struct timeval *now) {
    size_t          need = sizeof(struct icmphdr) + FT_PING_PAYLOAD_SIZE;
    struct icmphdr  *icmphdr = (struct icmphdr*)buffer;
    uint8_t         *payload = buffer + sizeof(struct icmphdr);
    struct timeval  *timeval = (struct timeval*)payload;
    
    if (buffer_len < need) {
        return 0;
    }

    memset(icmphdr, 0, need);

    icmphdr->type = ICMP_ECHO;
    icmphdr->code = 0;
    icmphdr->un.echo.id = htons(ident);
    icmphdr->un.echo.sequence = htons(seq);

    *timeval = *now;
    memset(payload + sizeof(struct timeval), 0x42, FT_PING_PAYLOAD_SIZE - sizeof(struct timeval));

    icmphdr->checksum = 0;
    icmphdr->checksum = icmp_checksum(buffer, need);

    return need;
}

bool parse_icmp_reply(const uint8_t *packet, ssize_t len, uint16_t ident, uint16_t *out_seq, uint8_t *out_ttl, double *out_rtts_ms) {
    const struct iphdr      *iphdr = (const struct iphdr*)packet;
    int                     iphdr_len = iphdr->ihl * 4;
    const struct icmphdr    *icmphdr = (const struct icmphdr*)(packet + iphdr_len);
    const uint8_t           *payload = (const uint8_t*)(icmphdr + 1);


    if (len < (ssize_t)(sizeof(struct iphdr) + sizeof(struct icmphdr))) { return false; }
    if (len < iphdr_len + (int)sizeof(struct icmphdr)) { return false; }
    if (icmphdr->type != ICMP_ECHOREPLY || icmphdr->code != 0) { return false; }
    if (ntohs(icmphdr->un.echo.id) != ident) { return false; }

    if (out_seq) { *out_seq = ntohs(icmphdr->un.echo.sequence); }
    if (out_ttl) { *out_ttl = iphdr->ttl; }

    if (len >= iphdr_len + (int)sizeof(struct icmphdr) + (int)(sizeof(struct timeval))) {
        struct timeval sent = *(const struct timeval*)payload;
        struct timeval now;

        gettimeofday(&now, NULL);

        if (out_rtts_ms) {
            *out_rtts_ms = timeval_diff_ms(&now, &sent);
        }
    }

    return true;
}