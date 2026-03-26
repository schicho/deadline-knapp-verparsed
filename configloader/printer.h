#ifndef PRINTER_H
#define PRINTER_H

#include <stdarg.h>
#include <stdio.h>

/*
These functions wrap the stdio functions to create another level of indirection.
Instead of printing using the stdio.h functions, use these.
*/

/* print to stdout */
int cfg_printf(const char* format, ...);
int cfg_vprintf(const char* format, va_list args);

/* print to stderr */
int cfg_err_printf(const char* format, ...);
int cfg_err_vprintf(const char* format, va_list args);

/* print to any file */
int cfg_fprintf(FILE* stream, const char* format, ...);
int cfg_vfprintf(FILE* stream, const char* format, va_list args);

#endif /* PRINTER_H */