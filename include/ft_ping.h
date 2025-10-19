#ifndef FT_PING
# define FT_PING

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <stdint.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <math.h>
# include <errno.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <netdb.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <float.h>

# define FT_PROG_NAME               "ft_ping"
# define FT_PING_PAYLOAD_SIZE       56
# define FT_PING_RECV_TIMEOUT_MS    1000
# define FT_PING_INTERVAL_MS        1000

typedef struct s_opts {
    bool        verbose;
    const char *target_str;
} t_opts;

typedef struct s_target {
    struct sockaddr_in  addr;
    char                fqdn[256]; // Nome dominio per esteso
    char                ip_str[INET_ADDRSTRLEN];
} t_target;

typedef struct s_stats {
    uint64_t    sent; // N Pacchetti inviati
    uint64_t    received;
    uint64_t    errors;
    double      rtt_min_ms; //Round Trip Time = ricezione - invio
    double      rtt_max_ms;
    double      rtt_sum_ms;
    double      rtt_sum2_ms;
} t_stats;

typedef struct s_env {
    int             sockfd;
    bool            running;
    uint16_t        ident; // PID processo
    uint16_t        seq; //Contatore progressivo pacchetti
    t_opts          opts;
    t_target        target;
    t_stats         stats;
    struct timeval  start_time;
} t_env;

/* core.c */
void ping_run(t_env *env);

/* icmp.c */
int     open_raw_socket(void);
void    set_socket_timeout(int fd, int timeout_ms);
ssize_t recv_packet(int fd, void *buffer, size_t buffer_len, struct sockaddr_in *from, struct timeval *timeval);
size_t  build_echo_request(uint8_t *buffer, size_t buffer_len, uint16_t ident, uint16_t seq, struct timeval *now);
bool parse_icmp_reply(const uint8_t *packet, ssize_t len, uint16_t ident, uint16_t *out_seq, uint8_t *out_ttl, double *out_rtts_ms);

/* resolve.c */
void resolve_target(const char *input, t_target *target);

/* args.c */
void parse_args(int argc, char **argv, t_opts *opts);
void print_usage(const char *program);

/* output.c */
void print_ping_header(const t_target *target);
void print_reply_line(const char *from_ip, size_t bytes, uint16_t seq, uint8_t ttl, double rtt_ms);
void print_timeout_line (const char *from_ip, uint16_t seq, bool verbose);
void print_verbose_icmp_notice(const char *msg);

/* stats.c */
void stats_init(t_stats *stats);
void stats_on_send(t_stats *stats);
void stats_on_recv(t_stats *stats, double rtt_ms);
void stats_on_error(t_stats *stats);
void stats_print_summary(const t_env *env);

/* utils.c */
uint16_t    icmp_checksum(const void *data, size_t len);
double      timeval_diff_ms(const struct timeval *end, const struct timeval *start);
void        xerror_die(const char *where, const char *why);


extern t_env *g_env;

#endif