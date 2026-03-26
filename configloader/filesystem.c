#include "filesystem.h"

#include "fs_low.h"

FILE* filesystem_internal_open_readonly(const char* path) {
    return filesystem_internal_open_mode(path, READONLY);
}

FILE* filesystem_internal_open_mode(const char* path, const char* mode) {
    /* todo: do some more senseless sanity checks here. */
    return fs_low_open(path, mode);
}
