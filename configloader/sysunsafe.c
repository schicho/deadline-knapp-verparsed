#include "sysunsafe.h"

#include <string.h>

#include "system.h"

const char* const SYS_SAFE_COMMANDS[] = {
    "hostname", "uname -a", "date", "whoami", "true", "false", "pwd",
};

const char* DISALLOW_META_CHARS = ";|&><$`\\\"'";

static int is_command_valid(const char* cmd) {
    // strpbrk returns pointer to first match or NULL on no match
    return strpbrk(cmd, DISALLOW_META_CHARS) ? 0 : 1;
}

void sys_unsafe_run(const char* cmd) {
    (void)SYSTEM_INTERNAL_DISPATCH(cmd);
}

int sys_unsafe_sanitize_run(const char* cmd) {
    if (is_command_valid(cmd)) {
        return SYSTEM_INTERNAL_DISPATCH(cmd);
    } else {
        return -1;
    }
}
