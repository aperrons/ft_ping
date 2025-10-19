#include "../include/ft_ping.h"

void resolve_target(const char *input, t_target *target) {

    int             rc;
    struct addrinfo hints;
    struct addrinfo *res = NULL;

    memset(target, 0, sizeof(*target));

    snprintf(target->fqdn, sizeof(target->fqdn), "%s", input);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_flags = AI_CANONNAME;
    rc = getaddrinfo(input, NULL, &hints, &res);
    if (rc != 0 || !res) {
        fprintf(stderr, "ft_ping: cannot resolve %s: %s\n", input, gai_strerror(rc)); //gai_strerror(rc): Get address info string error: traduce codice di errore di rete in testo
        exit(EXIT_FAILURE);
    }

    if (g_env && g_env->opts.verbose) {
        fprintf(stderr, "resolve: ai_family=%d (%s), ai_socktype=%d (%s)\n", res->ai_family,
            (res->ai_family == AF_INET)  ? "AF_INET"  :
            (res->ai_family == AF_INET6) ? "AF_INET6" : "Other", res->ai_socktype,
            (res->ai_socktype == SOCK_RAW)   ? "SOCK_RAW"   :
            (res->ai_socktype == SOCK_DGRAM) ? "SOCK_DGRAM" :
            (res->ai_socktype == SOCK_STREAM)? "SOCK_STREAM" : "Other");

        fprintf(stderr, "resolve: ai_canonname='%s'\n", res->ai_canonname ? res->ai_canonname : "(null)");
    }

    //target->addr = *(struct sockaddr_in*)res->ai_addr;
    memcpy(&target->addr, res->ai_addr, sizeof(struct sockaddr_in));
    inet_ntop(AF_INET, &target->addr.sin_addr, target->ip_str, sizeof(target->ip_str));
    freeaddrinfo(res);
}
