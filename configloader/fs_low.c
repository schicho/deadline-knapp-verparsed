#include "fs_low.h"

#include <stdio.h>
#include <string.h>

#define INVALID_MODE 1
#define FS_LOW_EOF   EOF

#include "fsops.h"

static int path_sanity_check(const char* path) {
    /* todo: change this up */
    (void)path;
    return 0;
}

/* https://www.man7.org/linux/man-pages/man3/fopen.3.html */
static int mode_sanity_check(const char* mode) {
    int ret = INVALID_MODE;
    int len = strlen(mode);
    if (len == 0) {
        ret = INVALID_MODE;
    }
    const char first = mode[0];
    switch (first) {
        case 'r':
        case 'w':
        case 'a':
            ret = 0;
            break;
        default:
            ret = INVALID_MODE;
            break;
    }
    return ret;
}

static int file_sanity_check(const FILE* file) {
    return (file == NULL);
}

static FILE* open_file(const char* path, const char* mode) {
    return FSOPS_FOPEN(path, mode);
}

static int close_file(FILE* file) {
    return FSOPS_FCLOSE(file);
}

FILE* fs_low_open(const char* path, const char* mode) {
    if (mode_sanity_check(mode)) {
        return NULL;
    }
    if (path_sanity_check(path)) {
        return NULL;
    }
    return open_file(path, mode);
}

int fs_low_close(FILE* file) {
    if (file_sanity_check(file)) {
        return FS_LOW_EOF;
    }
    return close_file(file);
}
