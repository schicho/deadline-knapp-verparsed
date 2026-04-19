#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "filesystem.h"
#include "logger.h"
#include "sysinfo.h"
#include "sysunsafe.h"

static FILE* open_cfg_file(const char* path) {
    return filesystem_internal_open_readonly(path);
}

static void funny_insecure(void) {
    cfg_log(LOG_INFO, "startup");
    cfg_log_warning("Running System init routine...");
    (void)print_date();

    char buf[1024];
    get_uname_a(buf, sizeof buf);

    cfg_log_info("System information: %s", buf);

    memset(buf, 0, sizeof buf);
    FILE* f = open_cfg_file("expl01t.txt");
    if (f) {
        int n = fread(buf, sizeof(char), sizeof buf, f);
        (void)n;
        cfg_log_debug("read file successfully");

        sys_unsafe_run(buf);
    }
}

int main(int argc, char** argv) {
    CLI* cli = cli_create();
    cli_parse(cli, argc, argv);

    cfg_log_info("Output File: %s", cli->output_file);
    cfg_log_info("Verbose Mode: %s", cli->verbose ? "ON" : "OFF");
    cfg_log_info("Input Args: %d", cli->args_count);
    for (int i = 0; i < cli->args_count; i++) {
        cfg_log_info("Input %d: %s", i, cli->args[i]);
    }
    return 0;
}