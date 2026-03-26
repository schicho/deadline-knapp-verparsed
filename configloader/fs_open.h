#ifndef FS_OPEN_H
#define FS_OPEN_H

#include <stdio.h>

#define FILE_OPEN(x, y) ({\
    FILE* r = NULL; \
    do { \
        r = fopen(x,y); \
    } while(0); \
    r; })

#endif /* FS_OPEN_H */
