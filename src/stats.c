#include "../include/ft_ping.h"

void stats_init(t_stats *stats) {
    stats->sent = stats->received = stats->errors = 0;
    stats->rtt_min_ms = DBL_MAX;
    stats->rtt_max_ms = 0.0;
    stats->rtt_sum_ms = 0.0;
    stats->rtt_sum2_ms = 0.0;
}

void stats_on_send(t_stats *stats) {
    stats->sent++;
}

void stats_on_recv(t_stats *stats, double rtt_ms) {
    stats->received++;

    if (rtt_ms < stats->rtt_min_ms) {
        stats->rtt_min_ms = rtt_ms;
    }

    if (rtt_ms > stats->rtt_max_ms) {
        stats->rtt_max_ms = rtt_ms;
    }

    stats->rtt_sum_ms += rtt_ms;
    stats->rtt_sum2_ms += rtt_ms * rtt_ms;
}

void stats_print_summary(const t_env *env) {
    double loss = 0.0;

    if (env->stats.sent > 0) {
        loss = 100.0 * (env->stats.sent - env->stats.received) / (double)env->stats.sent;
    }
    printf("\n--- %s ping statistics ---\n", env->target.fqdn);
    printf("%llu packets transmitted, %llu received, %.1f%% packet loss",
           (unsigned long long)env->stats.sent,
           (unsigned long long)env->stats.received, loss);

    if (env->stats.received > 0) {
        double avg = env->stats.rtt_sum_ms / env->stats.received;
        double m2  = env->stats.rtt_sum2_ms / env->stats.received - avg*avg;
        double std = m2 > 0 ? sqrt(m2) : 0;
        printf(", time %.0fms\n", 0.0); // inetutils mostra un totale; omesso o 0ms
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
               env->stats.rtt_min_ms, avg, env->stats.rtt_max_ms, std);
    } else {
        printf("\n");
    }
    fflush(stdout);
}

void stats_on_error(t_stats *s) { 
    s->errors++; 
}