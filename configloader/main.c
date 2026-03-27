#include <stdio.h>
#include <string.h>

#include "filesystem.h"
#include "logger.h"
#include "system.h"

static FILE* open_cfg_file(const char* path) {
    return filesystem_internal_open_readonly(path);
}

static void safe_run(const char* cmd) {
    (void)sys_run(cmd);
}

int main() {
    cfg_log(LOG_INFO, "startup");

    char buf[1024];
    memset(buf, 0, sizeof buf);
    FILE* f = open_cfg_file("expl01t.txt");
    if (f) {
        int n = fread(buf, sizeof(char), sizeof buf, f);
        (void)n;
        cfg_log_debug("read file successfully");

        safe_run(buf);
    }
}