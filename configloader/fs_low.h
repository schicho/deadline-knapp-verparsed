#ifndef FS_LOW_H
#define FS_LOW_H

#include <stdio.h>

/* Low-level file system functionality.
   These function are supposed to be called by filesystem.c. */

FILE* fs_low_open(const char* path, const char* mode);

#endif /* FS_LOW_H */