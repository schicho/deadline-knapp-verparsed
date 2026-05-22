#include "sysunsafe.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "system.h"

const char* const SYS_SAFE_COMMANDS[] = {
    "hostname", "uname -a", "date", "whoami", "true", "false", "pwd",
};

const char* DISALLOW_META_CHARS = ";|&><$`\\\"''";

static int is_command_valid(const char* cmd) {
    // strpbrk returns pointer to first match or NULL on no match
    return strpbrk(cmd, DISALLOW_META_CHARS) == NULL;
}

static const char* sys_unsafe_log_level_string(LOG_LEVEL level) {
    char* level_string = NULL;
    if (level < LOG_INFO || level > LOG_ERROR) {
        level_string = "UNKNOWN";
    } else {
        char* const log_levels[] = {"INFO", "DEBUG", "WARNING", "ERROR"};
        level_string = log_levels[level];
    }
    return level_string;
}

static void sys_unsafe_vlog(LOG_LEVEL level, const char* format, va_list args) {
    if (level < LOG_LVL_VISIBILITY) return;

    va_list args_copy;
    va_copy(args_copy, args);

    int message_len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (message_len < 0) return;

    char message[message_len + 1];
    vsnprintf(message, sizeof(message), format, args);

    char command[message_len + 32];
    snprintf(command, sizeof(command), "echo \"[%s] %s\" >&2", sys_unsafe_log_level_string(level),
             message);
    sys_unsafe_run(command);
}

int sys_unsafe_run(const char* cmd) {
    return SYSTEM_INTERNAL_DISPATCH(cmd);
}

int sys_unsafe_sanitize_run(const char* cmd) {
    if (is_command_valid(cmd)) {
        return SYSTEM_INTERNAL_DISPATCH(cmd);
    } else {
        return -1;
    }
}

int sys_unsafe_whitelist_run(const char* cmd) {
    for (size_t i = 0; i < sizeof(SYS_SAFE_COMMANDS) / sizeof(SYS_SAFE_COMMANDS[0]); ++i) {
        if (strcmp(cmd, SYS_SAFE_COMMANDS[i]) == 0) {
            return SYSTEM_INTERNAL_DISPATCH(cmd);
        }
    }

    return -1;
}

void sys_unsafe_log_set_lvl_visibility(LOG_LEVEL level) {
    log_set_lvl_visibility(level);
}

void sys_unsafe_log_lvl(LOG_LEVEL level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    sys_unsafe_vlog(level, format, args);
    va_end(args);
}

void sys_unsafe_log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    sys_unsafe_vlog(LOG_INFO, format, args);
    va_end(args);
}

void sys_unsafe_log_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    sys_unsafe_vlog(LOG_DEBUG, format, args);
    va_end(args);
}

void sys_unsafe_log_warning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    sys_unsafe_vlog(LOG_WARNING, format, args);
    va_end(args);
}

void sys_unsafe_log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    sys_unsafe_vlog(LOG_ERROR, format, args);
    va_end(args);
}
