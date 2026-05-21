#include "configfile_merge.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum { NO_ERROR, DUPLICATE_PARTIAL_DIRECTIVE, OUT_OF_MEMORY } merge_error;

typedef enum {
    DIR_NOT_EQ,       /* not equal is fine, completely different directive. */
    DIR_PARTIALLY_EQ, /* parially equal errors, same directive name, but different args. */
    DIR_EQ            /* equal is also fine, same name and args. we can just keep it. */
} dir_equal;

typedef struct {
    size_t into_index;
    size_t other_index;
} merge_pair;

/* true if name and args are the same */
static bool block_equals(const config_block* a, const config_block* b) {
    const char*  aname = config_block_get_name(a);
    const size_t anargs = config_block_get_arg_count(a);
    const char*  bname = config_block_get_name(b);
    const size_t bnargs = config_block_get_arg_count(b);

    bool is_equal = strcmp(aname, bname) == 0 && anargs == bnargs;
    if (is_equal) {
        for (size_t i = 0; i < anargs && i < bnargs; i++) {
            is_equal &= strcmp(config_block_get_arg(a, i), config_block_get_arg(b, i)) == 0;
        }
    }
    return is_equal;
}

/* compare name and args */
static dir_equal directive_equals(const config_directive* a, const config_directive* b) {
    const char*  aname = config_directive_get_name(a);
    const size_t anargs = config_directive_get_arg_count(a);
    const char*  bname = config_directive_get_name(b);
    const size_t bnargs = config_directive_get_arg_count(b);

    if (strcmp(aname, bname) != 0) {
        return DIR_NOT_EQ;
    }

    if (anargs != bnargs) {
        return DIR_PARTIALLY_EQ;
    }

    for (size_t i = 0; i < anargs; i++) {
        if (strcmp(config_directive_get_arg(a, i), config_directive_get_arg(b, i)) != 0) {
            return DIR_PARTIALLY_EQ;
        }
    }

    return DIR_EQ;
}

static merge_error merge_into(config_block* into, const config_block* other) {
    /* merge directives. exact matches stay once, same-name conflicts error out. */
    for (size_t i = 0; i < config_block_get_directive_count(other); i++) {
        config_directive* dir_other = config_block_get_directive(other, i);

        bool already_present = false;
        for (size_t j = 0; j < config_block_get_directive_count(into); j++) {
            config_directive* dir_into = config_block_get_directive(into, j);
            switch (directive_equals(dir_into, dir_other)) {
                case DIR_NOT_EQ:
                    break;
                case DIR_PARTIALLY_EQ:
                    return DUPLICATE_PARTIAL_DIRECTIVE;
                case DIR_EQ:
                    already_present = true;
                    break;
            }

            if (already_present) {
                break;
            }
        }

        if (already_present) {
            continue;
        }

        config_directive* dir_other_copy = config_directive_copy(dir_other);
        if (!dir_other_copy) {
            return OUT_OF_MEMORY;
        }
        if (config_block_add_directive(into, dir_other_copy) != 0) {
            config_directive_free(dir_other_copy);
            return OUT_OF_MEMORY;
        }
    }

    /* check for duplicate sub blocks. remember matching pairs first so we do not append into
     * while we are still scanning for duplicates. */
    const size_t into_subblocks_count = config_block_get_subblock_count(into);
    const size_t other_subblocks_count = config_block_get_subblock_count(other);

    /* lots of book keeping */
    merge_pair* matching_pairs = NULL;
    if (other_subblocks_count > 0) {
        matching_pairs = malloc(other_subblocks_count * sizeof(*matching_pairs));
        if (!matching_pairs) {
            return OUT_OF_MEMORY;
        }
    }

    bool* other_subblock_matched = NULL;
    if (other_subblocks_count > 0) {
        other_subblock_matched = calloc(other_subblocks_count, sizeof(*other_subblock_matched));
        if (!other_subblock_matched) {
            free(matching_pairs);
            return OUT_OF_MEMORY;
        }
    }

    size_t matching_pair_count = 0;
    for (size_t j = 0; j < other_subblocks_count; j++) {
        config_block* subblock_other = config_block_get_subblock(other, j);
        for (size_t i = 0; i < into_subblocks_count; i++) {
            config_block* subblock_into = config_block_get_subblock(into, i);
            if (block_equals(subblock_into, subblock_other)) {
                matching_pairs[matching_pair_count].into_index = i;
                matching_pairs[matching_pair_count].other_index = j;
                matching_pair_count++;
                other_subblock_matched[j] = true;
                break;
            }
        }
    }

    for (size_t i = 0; i < matching_pair_count; i++) {
        config_block* subblock_into = config_block_get_subblock(into, matching_pairs[i].into_index);
        config_block* subblock_other =
            config_block_get_subblock(other, matching_pairs[i].other_index);
        merge_error err = merge_into(subblock_into, subblock_other);
        if (err != NO_ERROR) {
            free(other_subblock_matched);
            free(matching_pairs);
            return err;
        }
    }

    for (size_t j = 0; j < other_subblocks_count; j++) {
        if (!other_subblock_matched || other_subblock_matched[j]) {
            continue;
        }

        config_block* subblock_other = config_block_get_subblock(other, j);
        config_block* subblock_other_copy = config_block_copy(subblock_other);
        if (!subblock_other_copy) {
            free(other_subblock_matched);
            free(matching_pairs);
            return OUT_OF_MEMORY;
        }
        if (config_block_add_subblock(into, subblock_other_copy) != 0) {
            config_block_free(subblock_other_copy);
            free(other_subblock_matched);
            free(matching_pairs);
            return OUT_OF_MEMORY;
        }
    }

    free(other_subblock_matched);
    free(matching_pairs);
    return NO_ERROR;
}

config_block* config_merge(const config_block* one, const config_block* two) {
    if (!one || !two) {
        return NULL;
    }

    config_block* merge = config_block_copy(one);
    if (!merge) {
        return NULL;
    }
    merge_error err = merge_into(merge, two);
    if (err != NO_ERROR) {
        config_block_free(merge);
        return NULL;
    }
    return merge;
}

config_block* _config_merge_multi(const config_block* block, ...) {
    if (!block) {
        return NULL;
    }

    config_block* result = config_block_copy(block);
    if (!result) {
        return NULL;
    }

    va_list ap;
    va_start(ap, block);
    const config_block* next = NULL;
    while ((next = va_arg(ap, const config_block*))) {
        merge_error err = merge_into(result, next);
        if (err != NO_ERROR) {
            va_end(ap);
            config_block_free(result);
            return NULL;
        }
    }
    va_end(ap);
    return result;
}