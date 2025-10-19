#include "../include/ft_ping.h"

t_env *g_env = NULL;

int main(int argc, char **argv) {

    t_env env;

    memset(&env, 0, sizeof(env));
    g_env = &env;

    parse_args(argc, argv, &env.opts);

    resolve_target(env.opts.target_str, &env.target);

    env.sockfd = open_raw_socket();

    set_socket_timeout(env.sockfd, FT_PING_RECV_TIMEOUT_MS);

    env.ident = (uint16_t)getpid();

    stats_init(&env.stats);
    env.running = true;
    gettimeofday(&env.start_time, NULL);

    ping_run(&env);

    //close(env.sockfd);

    return 0;
}

