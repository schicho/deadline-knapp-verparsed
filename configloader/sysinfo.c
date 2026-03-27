#include "sysinfo.h"

#include <stddef.h>

#include "stdio.h"
#include "system.h"

int print_date(void) {
    return SYSTEM_INTERNAL_DISPATCH("date");
}

int print_uname_a(void) {
    return SYSTEM_INTERNAL_DISPATCH("uname -a");
}

int get_uname_a(char* buf, size_t len) {
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
    return SYSTEM_INTERNAL_RUN_STOP(p);
}
