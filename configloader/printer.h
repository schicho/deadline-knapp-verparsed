#ifndef PRINTER_H
#define PRINTER_H

#include <stdarg.h>
#include <stdio.h>

int cfg_printf(const char* format, ...);
int cfg_vprintf(const char* format, va_list args);

int cfg_err_printf(const char* format, ...);
int cfg_err_vprintf(const char* format, va_list args);

int cfg_fprintf(FILE* stream, const char* format, ...);
int cfg_vfprintf(FILE* stream, const char* format, va_list args);

#endif /* PRINTER_H */