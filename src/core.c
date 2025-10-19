#include "../include/ft_ping.h"

static void on_sigint(int sig) {
    (void)sig;

    printf("\n--- Interrupted by user (SIGINT) ---\n");

    g_env->running = false;

    fflush(stdout);
}

static void send_probe(t_env *env) {
    uint8_t         packet[sizeof(struct icmphdr) + FT_PING_PAYLOAD_SIZE];
    struct timeval  now;
    size_t          packet_size;
    ssize_t         sent;

    gettimeofday(&now, NULL);

    packet_size = build_echo_request(packet, sizeof(packet), env->ident, env->seq, &now);
    if (packet_size == 0) {
        fprintf(stderr, "ft_ping: failed to build packet\n");
        stats_on_error(&env->stats);
        return;
    }

    sent = sendto(env->sockfd, packet, packet_size, 0, (struct sockaddr*)&env->target.addr, sizeof(env->target.addr));
    if (sent < 0) {
        perror("ft_ping: sendto");
        stats_on_error(&env->stats);
        return;
    }

    stats_on_send(&env->stats);

    if (env->opts.verbose) {
        fprintf(stderr, "Sent ICMP Echo Request: seq=%u ident=%u size=%zu bytes to %s\n",
            env->seq, env->ident, packet_size, env->target.ip_str);
    }
}

static void handle_packet(t_env *env, uint8_t *buffer, ssize_t len, struct sockaddr_in *from) {
    const struct iphdr      *iphdr = (struct iphdr*)buffer;
    const struct icmphdr    *icmphdr = (struct icmphdr*)(buffer + iphdr->ihl * 4);
    char                    from_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &from->sin_addr, from_ip, sizeof(from_ip));

    switch(icmphdr->type) {
        case ICMP_ECHOREPLY: {
            uint16_t    seq = 0;
            uint8_t     ttl = iphdr->ttl;
            double      rtt = 0.0;

            if (parse_icmp_reply(buffer, len, env->ident, &seq, &ttl, &rtt)) {
                print_reply_line(from_ip, len - iphdr->ihl * 4, seq, ttl, rtt);
                stats_on_recv(&env->stats, rtt);

                if (env->opts.verbose) {
                    fprintf(stderr, "verbose: icmp_seq=%u ident=%u ttl=%u bytes=%zd rtt=%.3f ms\n",
                    seq, env->ident, ttl, len - iphdr->ihl * 4, rtt);
                }
            }
            break;
        }
        case ICMP_TIME_EXCEEDED:
            if (env->opts.verbose) {
                fprintf(stderr, "From %s icmp_seq=%u Time to live exceeded\n",
                        from_ip, env->seq);
            }   
            stats_on_error(&env->stats);
            break;

        case ICMP_DEST_UNREACH:
            if (env->opts.verbose) {
                fprintf(stderr, "From %s icmp_seq=%u Destination unreachable\n",
                        from_ip, env->seq);
            }
                
            stats_on_error(&env->stats);
            break;

        default:
            if (env->opts.verbose) {
                fprintf(stderr, "ft_ping: unexpected ICMP type=%d code=%d\n",
                        icmphdr->type, icmphdr->code);
            }
                
            break;
    }
}

void ping_run(t_env *env)
{
    signal(SIGINT, on_sigint);

    print_ping_header(&env->target);

    while (env->running)
    {
        env->seq++;

        send_probe(env);

        uint8_t             buffer[1500];
        struct sockaddr_in  from;
        struct timeval      timeval;

        ssize_t n = recv_packet(env->sockfd, buffer, sizeof(buffer), &from, &timeval);

        if (n < 0) {
            print_timeout_line(env->target.ip_str, env->seq, env->opts.verbose);
        } else {
            handle_packet(env, buffer, n, &from);
        }

        usleep(FT_PING_INTERVAL_MS * 1000);
    }

    stats_print_summary(env);
}