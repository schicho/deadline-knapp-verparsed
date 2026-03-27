#define _POSIX_C_SOURCE 200809L /* for popen and pclose */
#include "system.h"

#include <stdio.h>
#include <stdlib.h>

int sys_run(const char* cmd) {
    return system(cmd);
}

FILE* sys_run_pipe(const char* cmd, const char* type) {
    return popen(cmd, type);
}

int sys_close_pipe(FILE* stream) {
    return pclose(stream);
}
