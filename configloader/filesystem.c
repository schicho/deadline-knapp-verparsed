#include "filesystem.h"

#include "fs_low.h"

FILE* filesystem_open_readonly(const char* path) {
    return filesystem_open(path, READONLY);
}

FILE* filesystem_open(const char* path, const char* mode) {
    /* todo: do some more senseless sanity checks here. */
    return fs_low_open(path, mode);
}

int filesystem_close(FILE* file) {
    return fs_low_close(file);
}
