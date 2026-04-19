#include "cli.h"

#include <argp.h>
#include <stdbool.h>
#include <stdlib.h>

#define PROGRAM_NAME_VERSION "configloader 6.7"

/* global metadata variable */
const char* argp_program_version = PROGRAM_NAME_VERSION;

static char doc[] = "configloader 6.7 - load and merge nginx configuration files";
static char args_doc[] = "[FILE]...";

/* {name, key, arg, flags, doc, group} */
static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Verbose output", 0},
    {"output", 'o', "FILE", 0, "Output to FILE instead of standard output", 0},
    {0}};

static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    struct CLI* cli = state->input;

    switch (key) {
        case 'v':
            cli->verbose = true;
            break;
        case 'o':
            cli->output_file = arg;
            break;
        case ARGP_KEY_ARG:
            /* positional args */
            if (cli->args_count >= cli->args_alloc) {
                int    na = cli->args_alloc ? cli->args_alloc * 2 : 16;
                char** nb = realloc(cli->args, (size_t)na * sizeof(char*));
                if (!nb) {
                    /* could not allocate enough */
                    argp_usage(state);
                }
                cli->args = nb;
                cli->args_alloc = na;
            }
            cli->args[cli->args_count] = arg;
            cli->args_count++;
            break;
        case ARGP_KEY_END:
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

void cli_parse(CLI* cli, int argc, char** argv) {
    cli->verbose = false;
    cli->output_file = "-"; /* default stdout */

    argp_parse(&argp, argc, argv, 0, 0, cli);
}

CLI* cli_create(void) {
    CLI* cli = calloc(1, sizeof(CLI));
    if (!cli) {
        return NULL;
    }
    cli->args_alloc = 16;
    cli->args = calloc((size_t)cli->args_alloc, sizeof(char*));
    if (!cli->args) {
        free(cli);
        return NULL;
    }
    return cli;
}

void cli_free(CLI* cli) {
    free(cli->args);
    free(cli);
}
