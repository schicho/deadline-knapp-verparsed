#ifndef SYSINFO_H
#define SYSINFO_H

#include <stddef.h>

int print_date(void);
int print_uname_a(void);
int get_uname_a(char* buf, size_t len);

#endif /* SYSINFO_H */