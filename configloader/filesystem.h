#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define READONLY       "r"
#define READWRITE      "r+"
#define WRITEONLYTRUNC "w"
#define READWRITETRUNC "w+"
#define APPENDONLY     "a"
#define APPENDWRITE    "a+"

#include <stdio.h>

/* High-level file system functionality */

FILE* filesystem_internal_open_readonly(const char* path);
FILE* filesystem_internal_open_mode(const char* path, const char* mode);

int filesystem_internal_close(FILE* file);

#endif /* FILESYSTEM_H */