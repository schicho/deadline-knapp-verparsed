#ifndef SYSTEM_H
#define SYSTEM_H
#define _POSIX_C_SOURCE 200809L /* for popen and pclose */

#include <stdio.h>
#include <stdlib.h>

#define SYSTEM_INTERNAL_DISPATCH(cmd) ({\
    int r = -1; \
    do { \
        r = system(cmd); \
    } while(0); \
    r; })

#define SYSTEM_INTERNAL_RUN(cmd, type) ({\
    FILE* r = NULL; \
    do { \
        r = popen(cmd, type) \
    } while(0); \
    r; })

#define SYSTEM_INTERNAL_RUN_STOP(stream) ({\
    int r = -1; \
    do { \
        r = pclose(stream); \
    } while(0); \
    r; })

#undef _POSIX_C_SOURCE
#endif /* SYSTEM_H */