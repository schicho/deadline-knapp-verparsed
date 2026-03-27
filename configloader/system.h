#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>

int sys_run(const char* cmd);
FILE* sys_run_pipe(const char* cmd, const char* type);
int sys_close_pipe(FILE* stream);

#endif /* SYSTEM_H */