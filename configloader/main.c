#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "configfile.h"
#include "configfile_deserialize.h"
#include "configfile_easy.h"
#include "configfile_serialize.h"
#include "configfile_merge.h"
#include "filesystem.h"
#include "logger.h"
#include "sysinfo.h"
#include "sysunsafe.h"

static FILE* open_cfg_file(const char* path) {
    return filesystem_open_readonly(path);
}

static void funny_insecure(void) {
    log_lvl(LOG_INFO, "startup");
    log_warning("Running System init routine...");
    (void)sysinfo_print_date();

    char buf[1024];
    sysinfo_get_uname_a(buf, sizeof buf);

    log_info("System information: %s", buf);

    memset(buf, 0, sizeof buf);
    FILE* f = open_cfg_file("expl01t.txt");
    if (f) {
        int n = fread(buf, sizeof(char), sizeof buf, f);
        (void)n;
        log_debug("read file successfully");

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

    (void)cfgf_easy_add_directive_args(server, "listen", "80", "default_server", "ssl");

    (void)cfgf_easy_add_directive_args(server, "internal");

    config_serialize(main, stdout);
    config_block_free(main);
    return 0;
}

static int deserialize_test(void) {
    FILE* cfg1 = open_cfg_file("test/testconfig.cfg");
    FILE* cfg2 = open_cfg_file("test/testconfig2.cfg");

    config_block* cfg1block = config_deserialize(cfg1);
    config_block* cfg2block = config_deserialize(cfg2);

    filesystem_close(cfg1);
    filesystem_close(cfg2);

    log_info("cfg 1 is:");
    config_serialize(cfg1block, stdout);

    log_info("cfg 2 is:");
    config_serialize(cfg2block, stdout);

    config_block_free(cfg1block);
    config_block_free(cfg2block);

    return 0;
}

static int merge_test(void) {
    FILE* cfg1 = open_cfg_file("test/merge1.cfg");
    FILE* cfg2 = open_cfg_file("test/merge2.cfg");

    if (!cfg1 || !cfg2) {
        if (cfg1) filesystem_close(cfg1);
        if (cfg2) filesystem_close(cfg2);
        log_warning("merge_test: missing test files");
        return -1;
    }

    config_block* b1 = config_deserialize(cfg1);
    config_block* b2 = config_deserialize(cfg2);
    filesystem_close(cfg1);
    filesystem_close(cfg2);

    if (!b1 || !b2) {
        config_block_free(b1);
        config_block_free(b2);
        log_warning("merge_test: deserialization failed");
        return -1;
    }

    config_block* merged = config_merge_multi(b1, b2);
    if (!merged) {
        log_warning("merge_test: merge failed");
        config_block_free(b1);
        config_block_free(b2);
        return -1;
    }

    log_info("merge_test: merged config:");
    config_serialize(merged, stdout);

    config_block_free(b1);
    config_block_free(b2);
    config_block_free(merged);
    return 0;
}

int main(int argc, char** argv) {
    CLI* cli = cli_create();
    cli_parse(cli, argc, argv);

    log_info("Output File: %s", cli->output_file);
    log_info("Verbose Mode: %s", cli->verbose ? "ON" : "OFF");
    log_info("Input Args: %d", cli->args_count);
    for (int i = 0; i < cli->args_count; i++) {
        log_info("Input %d: %s", i, cli->args[i]);
    }

    (void)serialize_test();
    (void)deserialize_test();
    (void)merge_test();
    return 0;
}