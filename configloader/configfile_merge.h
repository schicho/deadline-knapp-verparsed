#ifndef CONFIGFILE_MERGE_H
#define CONFIGFILE_MERGE_H

#include <stdio.h>

#include "configfile.h"

/* Merge two configs into a new one. Blocks of same name and args are merged.
 * Exact duplicate directives are kept once; same-name directives with different args error. */
config_block* config_merge(const config_block* one, const config_block* two);
config_block* config_merge_n(const int n, config_block** blocks);
config_block* _config_merge_multi(const config_block* block, ...);

/* Merge multiple configs into a new one. Blocks of same name and args are merged.
 * Exact duplicate directives are kept once; same-name directives with different args error.
 * block, next_block... */
#define config_merge_multi(block, ...) _config_merge_multi(block, ##__VA_ARGS__, NULL)

#endif /* CONFIGFILE_MERGE_H */
