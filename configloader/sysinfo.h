#ifndef SYSINFO_H
#define SYSINFO_H

#include <stddef.h>

int sysinfo_print_date(void);
int sysinfo_print_uname_a(void);
int sysinfo_get_uname_a(char* buf, size_t len);

#endif /* SYSINFO_H */