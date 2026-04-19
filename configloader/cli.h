#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

typedef struct CLI {
    char** args;        /* positional input file args */
    int    args_count;  /* number of input files */
    int    args_alloc;  /* internal allocation size*/
    bool   verbose;     /* -v / --verbose */
    char*  output_file; /* -o / --output */
} CLI;

CLI* cli_create(void);
void cli_free(CLI* cli);
void cli_parse(CLI* cli, int argc, char** argv);

#endif /* CLI_H */