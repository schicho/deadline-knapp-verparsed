#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "configfile.h"
#include "configfile_easy.h"
#include "configfile_serialize.h"
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
    (void)sysinfo_print_date();

    char buf[1024];
    sysinfo_get_uname_a(buf, sizeof buf);

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

static int serialize_test(void) {
    config_block*     main = cfgf_easy_new_main_block();
    config_directive* user = cfgf_easy_add_directive(main, "user");
    config_directive_add_arg(user, "nobody");

    (void)cfgf_easy_add_directive(main, "internal");

    config_block*     http = cfgf_easy_add_subblock(main, "http");
    config_directive* http_error_log = cfgf_easy_add_directive(http, "error_log");
    config_directive_add_arg(http_error_log, "logs/error.log");
    config_directive_add_arg(http_error_log, "notice");

    config_block* server = cfgf_easy_add_subblock(http, "server");
    config_block* location1 = cfgf_easy_add_subblock(server, "location");
    config_block_add_arg(location1, "/one");

    (void)cfgf_easy_add_directive_args(server, "listen", "80",
                                       "default_server",
                                       "ssl");

    (void)cfgf_easy_add_directive_args(server, "internal");

    config_serialize(main, stdout);
    config_block_free(main);
    return 0;
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

    (void)serialize_test();
    return 0;
}