#include "filesystem.h"

#include <ctype.h>
#include <string.h>

#include "fs_low.h"
#include "logger.h"

static int path_sanity_check(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return 1;
    }

    int saw_non_space = 0;
    for (const unsigned char* current = (const unsigned char*)path; *current != '\0'; ++current) {
        if (iscntrl(*current)) {
            return 1;
        }
        if (!isspace(*current)) {
            saw_non_space = 1;
        }
    }

    return !saw_non_space;
}

static int mode_sanity_check(const char* mode) {
    if (mode == NULL || mode[0] == '\0') {
        return 1;
    }

    const char* const valid_modes[] = {
        READONLY, READWRITE, WRITEONLYTRUNC, READWRITETRUNC, APPENDONLY, APPENDWRITE,
    };

    for (unsigned int index = 0; index < sizeof(valid_modes) / sizeof(valid_modes[0]); ++index) {
        if (strcmp(mode, valid_modes[index]) == 0) {
            return 0;
        }
    }

    return 1;
}

static int file_sanity_check(const FILE* file) {
    return (file == NULL);
}

FILE* filesystem_open_readonly(const char* path) {
    return filesystem_open(path, READONLY);
}

FILE* filesystem_open(const char* path, const char* mode) {
    log_debug("filesystem_open: path='%s', mode='%s'", path != NULL ? path : "(null)",
              mode != NULL ? mode : "(null)");

    if (path_sanity_check(path)) {
        log_error("can not open file of invalid path");
        return NULL;
    }

    if (mode_sanity_check(mode)) {
        log_error("can not open file with invalid mode '%s'", mode != NULL ? mode : "(null)");
        return NULL;
    }

    FILE* file = fs_low_open(path, mode);
    if (file == NULL) {
        log_error("failed to open path='%s' with mode='%s'", path, mode);
        return NULL;
    }

    log_info("opened path='%s' with mode='%s'", path, mode);
    return file;
}

int filesystem_close(FILE* file) {
    if (file_sanity_check(file)) {
        log_error("filesystem_close: invalid file handle");
        return EOF;
    }

    int result = fs_low_close(file);
    if (result == EOF) {
        log_error("filesystem_close: close failed for file handle %p", (void*)file);
        return result;
    }

    log_debug("filesystem_close: closed handle=%p", (void*)file);
    return result;
}
