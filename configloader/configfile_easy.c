#include "configfile_easy.h"

#include <stdarg.h>
#include <stddef.h>

#define MAIN_BLOCK_NAME "main"

config_block* cfgf_easy_new_main_block(void) {
    config_block* main_block = config_block_create(MAIN_BLOCK_NAME);
    return main_block;
}

config_directive* cfgf_easy_add_directive(config_block* block, const char* name) {
    if (!block) {
        return NULL;
    }

    config_directive* directive = config_directive_create(name);
    if (!directive) {
        return NULL;
    }

    if (config_block_add_directive(block, directive) != 0) {
        config_directive_free(directive);
        return NULL;
    }
    return directive;
}

config_directive* _cfgf_easy_add_directive_args(config_block* block, const char* name, ...) {
    if (!block) {
        return NULL;
    }

    config_directive* directive = config_directive_create(name);
    if (!directive) {
        return NULL;
    }

    va_list args;
    va_start(args, name);

    for (const char* arg = va_arg(args, const char*); arg != NULL;
         arg = va_arg(args, const char*)) {
        if (config_directive_add_arg(directive, arg) != 0) {
            va_end(args);
            config_directive_free(directive);
            return NULL;
        }
    }

    va_end(args);

    if (config_block_add_directive(block, directive) != 0) {
        config_directive_free(directive);
        return NULL;
    }

    return directive;
}

config_block* _cfgf_easy_add_subblock_args(config_block* block, const char* name, ...) {
    if (!block) {
        return NULL;
    }

    config_block* subblock = config_block_create(name);
    if (!subblock) {
        return NULL;
    }

    va_list args;
    va_start(args, name);

    for (const char* arg = va_arg(args, const char*); arg != NULL;
         arg = va_arg(args, const char*)) {
        if (config_block_add_arg(subblock, arg) != 0) {
            va_end(args);
            config_block_free(subblock);
            return NULL;
        }
    }

    va_end(args);

    if (config_block_add_subblock(block, subblock) != 0) {
        config_block_free(subblock);
        return NULL;
    }

    return subblock;
}

config_block* cfgf_easy_add_subblock(config_block* block, const char* name) {
    if (!block) {
        return NULL;
    }

    config_block* subblock = config_block_create(name);
    if (!subblock) {
        return NULL;
    }
    if (config_block_add_subblock(block, subblock) != 0) {
        config_block_free(subblock);
        return NULL;
    }
    return subblock;
}
