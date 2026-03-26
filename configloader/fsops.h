#ifndef FSOPS_H
#define FSOPS_H

#include <stdio.h>

#define FSOPS_FOPEN(x, y) ({\
    FILE* r = NULL; \
    do { \
        r = fopen(x,y); \
    } while(0); \
    r; })

#endif /* FSOPS_H */
