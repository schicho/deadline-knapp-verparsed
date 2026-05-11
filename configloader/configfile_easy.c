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
