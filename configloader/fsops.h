#ifndef FSOPS_H
#define FSOPS_H

#include <stdio.h>

#define FSOPS_FOPEN(p, m) ({\
    FILE* r = NULL; \
    do { \
        r = fopen(p,m); \
    } while(0); \
    r; })

#define FSOPS_FCLOSE(f) ({\
    int r = -1; \
    do { \
        r = fclose(f); \
    } while(0); \
    r; })

#endif /* FSOPS_H */
