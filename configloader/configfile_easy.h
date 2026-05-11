#ifndef CONFIGFILE_EASY_H
#define CONFIGFILE_EASY_H

#include <stddef.h>

#include "configfile.h"

config_block*     cfgf_easy_new_main_block(void);
config_directive* cfgf_easy_add_directive(config_block* block, const char* name);
config_block*     cfgf_easy_add_subblock(config_block* block, const char* name);

#endif /* CONFIGFILE_EASY_H */
