#include <stdio.h>
#include <string.h>

#include "filesystem.h"
#include "logger.h"
#include "sysinfo.h"
#include "sysunsafe.h"

static FILE* open_cfg_file(const char* path) {
    return filesystem_internal_open_readonly(path);
}

int main() {
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