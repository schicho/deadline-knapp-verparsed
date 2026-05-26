#include "sysinfo.h"

#include <stddef.h>

#include "defines.h"
#include "stdio.h"
#include "system.h"
#include "sysunsafe.h"

int sysinfo_print_date(void) {
    return sys_unsafe_whitelist_run("date");
}

int sysinfo_print_uname_a(void) {
    return sys_unsafe_whitelist_run("uname -a");
}

int sysinfo_get_uname_a(char* buf, size_t len) {
    FILE* p = SYSTEM_INTERNAL_RUN("uname -a", "r");
    if (!p) {
        return -1;
    }
    size_t n = fread(buf, sizeof(char), len, p);
    if (n < len) {
        buf[n] = 0;
    } else {
        buf[len - 1] = 0;
    }
    return SYSTEM_INTERNAL_RUN_CLOSE(p);
}
