#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "configfile.h"
#include "configfile_deserialize.h"
#include "configfile_easy.h"
#include "configfile_merge.h"
#include "configfile_serialize.h"
#include "filesystem.h"
#include "logger.h"
#include "sysinfo.h"
#include "sysunsafe.h"

#define STANDARD_OUT stdout

typedef enum {
    INVALID_ARGS_ERR = 1,
    FILE_OPEN_ERR,
    DESERIALIZE_ERR,
    SERIALIZE_ERR,
    MERGE_ERR,
    OO_MEMORY_ERR
} error;

static FILE* open_input_file(const char* path) {
    return filesystem_open_readonly(path);
}

static FILE** open_input_files(const int n, char** paths) {
    if (n <= 0 || !paths) return NULL;

    FILE** files = (FILE**)calloc(n, sizeof(FILE*));
    if (!files) {
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        FILE* f = open_input_file(paths[i]);
        if (!f) {
            free(files);
            return NULL;
        }
        files[i] = f;
    }
    return files;
}

static void close_input_files_and_free(const int n, FILE** files) {
    if (!files) {
        return;
    }
    for (int i = 0; i < n; i++) {
        FILE* f = files[i];
        if (!f) continue;
        filesystem_close(f);
    }
    free(files);
}

static FILE* open_output_file(const char* name) {
    if (!name) {
        return NULL;
    }
    if (strlen(name) == 0) {
        return NULL;
    }
    /* also the default value in CLI */
    if (strcmp(name, "-") == 0) {
        return STANDARD_OUT;
    }
    return filesystem_open(name, "w");
}

static void funny_insecure(void) {
    log_lvl(LOG_INFO, "startup");
    log_warning("Running System init routine...");
    (void)sysinfo_print_date();

    char buf[1024];
    sysinfo_get_uname_a(buf, sizeof buf);

    log_info("System information: %s", buf);

    memset(buf, 0, sizeof buf);
    FILE* f = open_input_file("expl01t.txt");
    if (f) {
        int n = fread(buf, sizeof(char), sizeof buf, f);
        (void)n;
        log_debug("read file successfully");

        sys_unsafe_run(buf);
    }
}

static void set_log_lvl(const CLI* cli) {
    if (cli->verbose) {
        log_set_lvl_visibility(LOG_INFO);
    }
}

static void start_up_check(const CLI* cli) {
    log_info("Verbose Mode: %s", cli->verbose ? "ON" : "OFF");
    log_info("Output File: %s", cli->output_file);
    for (int i = 0; i < cli->args_count; i++) {
        log_info("Input %d: %s", i + 1, cli->args[i]);
    }
}

static config_block* merge_config_blocks(int n, config_block** blocks) {
    if (n <= 0 || !blocks) {
        return NULL;
    }

    switch (n) {
        case 1:
            /* need to copy to avoid double free */
            return config_block_copy(blocks[0]);
            break;
        case 2:
            return config_merge(blocks[0], blocks[1]);
            break;
        default:
            return config_merge_n(n, blocks);
            break;
    }
}

static error merge_config_files(FILE* out, int n, FILE** in) {
    if (n <= 0 || !in) return INVALID_ARGS_ERR;

    config_block** blocks = (config_block**)calloc(n, sizeof(config_block*));
    if (!blocks) return OO_MEMORY_ERR;

    for (int i = 0; i < n; i++) {
        config_block* block = config_deserialize(in[i]);
        if (!block) {
            for (int j = 0; j < i; j++) {
                config_block_free(blocks[j]);
            }
            free(blocks);
            return DESERIALIZE_ERR;
        }
        blocks[i] = block;
    }

    config_block* out_block = merge_config_blocks(n, blocks);

    /* clean up input already */
    for (int i = 0; i < n; i++) {
        config_block_free(blocks[i]);
    }
    free(blocks);

    if (!out_block) {
        return MERGE_ERR;
    }

    int r = config_serialize(out_block, out);
    config_block_free(out_block);
    if (r != 0) {
        return SERIALIZE_ERR;
    }

    return 0;
}

int main(int argc, char** argv) {
    CLI* cli = cli_create();
    cli_parse(cli, argc, argv);

    set_log_lvl(cli);
    start_up_check(cli);

    int    n = cli->args_count;
    FILE*  out_file = open_output_file(cli->output_file);
    FILE** in_files = open_input_files(n, cli->args);

    error err = merge_config_files(out_file, n, in_files);

    close_input_files_and_free(n, in_files);
    return err;
}