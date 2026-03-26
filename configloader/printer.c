#include "printer.h"

#include <stdio.h>

#define CFG_STDERR stderr

int cfg_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = cfg_vprintf(format, args);
    va_end(args);
    return ret;
}

int cfg_vprintf(const char* format, va_list args) {
    return vprintf(format, args);
}

int cfg_err_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = cfg_err_vprintf(format, args);
    va_end(args);
    return ret;
}

int cfg_err_vprintf(const char* format, va_list args) {
    return cfg_vfprintf(CFG_STDERR, format, args);
}

int cfg_fprintf(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = cfg_vfprintf(stream, format, args);
    va_end(args);
    return ret;
}

int cfg_vfprintf(FILE* stream, const char* format, va_list args) {
    return vfprintf(stream, format, args);
}
