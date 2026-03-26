#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

static const char* log_level_string(LOG_LEVEL level) {
    char* level_string = NULL;
    if (level < LOG_INFO || level > LOG_ERROR) {
        level_string = "UNKNOWN";
    } else {
        char* const log_levels[] = {"INFO", "DEBUG", "WARNING", "ERROR"};
        level_string = log_levels[level];
    }
    return level_string;
}

static void vcfg_log(LOG_LEVEL level, const char* format, va_list args) {
    const char* level_str = log_level_string(level);
    fprintf(stderr, "[%s] ", level_str);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}

void cfg_log(LOG_LEVEL level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcfg_log(level, format, args);
    va_end(args);
}

void cfg_log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcfg_log(LOG_INFO, format, args);
    va_end(args);
}

void cfg_log_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcfg_log(LOG_DEBUG, format, args);
    va_end(args);
}

void cfg_log_warning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcfg_log(LOG_WARNING, format, args);
    va_end(args);
}

void cfg_log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcfg_log(LOG_ERROR, format, args);
    va_end(args);
}
