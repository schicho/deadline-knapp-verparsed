#include "logger.h"

#include <stdarg.h>

#include "printer.h"

static LOG_LEVEL LOG_LVL_VISIBILITY = LOG_WARNING;

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

static void vlog(LOG_LEVEL level, const char* format, va_list args) {
    if (level < LOG_LVL_VISIBILITY) return;

    const char* level_str = log_level_string(level);
    cfg_err_printf("[%s] ", level_str);
    cfg_err_vprintf(format, args);
    cfg_err_printf("\n");
}

void log_set_lvl_visibility(LOG_LEVEL level) {
    LOG_LVL_VISIBILITY = level;
}

void log_lvl(LOG_LEVEL level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(level, format, args);
    va_end(args);
}

void log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(LOG_INFO, format, args);
    va_end(args);
}

void log_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(LOG_DEBUG, format, args);
    va_end(args);
}

void log_warning(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(LOG_WARNING, format, args);
    va_end(args);
}

void log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vlog(LOG_ERROR, format, args);
    va_end(args);
}
