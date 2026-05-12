#ifndef CONFIGFILE_EASY_H
#define CONFIGFILE_EASY_H

#include <stddef.h>

#include "configfile.h"

config_block*     cfgf_easy_new_main_block(void);
config_directive* cfgf_easy_add_directive(config_block* block, const char* name);
config_block*     cfgf_easy_add_subblock(config_block* block, const char* name);

config_directive* _cfgf_easy_add_directive_args(config_block* block, const char* name, ...);
config_block*     _cfgf_easy_add_subblock_args(config_block* block, const char* name, ...);

/* block, name, args... */
#define cfgf_easy_add_directive_args(block, name, ...) \
    _cfgf_easy_add_directive_args(block, name, ##__VA_ARGS__, NULL)

/* block, name, args */
#define cfgf_easy_add_subblock_args(block, name, ...) \
    _cfgf_easy_add_subblock_args(block, name, ##__VA_ARGS__, NULL)

#endif /* CONFIGFILE_EASY_H */
