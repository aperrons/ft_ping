#include "../include/ft_ping.h"

void print_usage(const char *prog_name) {
    fprintf(stderr,
        "Usage: %s [-v] [-?] destination\n"
        "Send ICMP ECHO_REQUEST to network host.\n\n"
        "   -v      verbose output\n"
        "   -?      show this help and exit\n",
        prog_name);
}

static void ensure_target(const t_opts *opts) {
    if (!opts->target_str) {
        fprintf(stderr, "ft_ping: missing destination address\n");
        print_usage(FT_PROG_NAME);
        exit(EXIT_FAILURE);
    }
}

void parse_args(int argc, char **argv, t_opts *opts) {
    memset(opts, 0, sizeof(*opts));

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0) {
            opts->verbose = true;
        } else if (strcmp(argv[i], "-?") == 0) {
            print_usage(FT_PROG_NAME);
            exit(EXIT_SUCCESS);
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "ft_ping: invalid option -- '%s'\n", argv[i]);
            print_usage(FT_PROG_NAME);
            exit(EXIT_FAILURE);
        } else {
            if (opts->target_str) {
                fprintf(stderr, "ft_ping: extra argument '%s'\n", argv[i]);
                print_usage(FT_PROG_NAME);
                exit(EXIT_FAILURE);
            }
            opts->target_str = argv[i];
        }
    }

    ensure_target(opts);
}