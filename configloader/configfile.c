#include "configfile.h"

#include <stdlib.h>
#include <string.h>

struct config_directive {
    char*  name;
    char** args;
    size_t arg_count;
};

struct config_block {
    char*  name;
    char** args;
    size_t arg_count;

    struct config_directive** directives;
    size_t                    directive_count;

    struct config_block** subblocks;
    size_t                subblock_count;
};

static char* config_strdup(const char* s) {
    if (!s) {
        return NULL;
    }

    size_t len = strlen(s) + 1;
    char*  copy = (char*)malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}

/**
 * Reallocate an array to add one more element.
 * Returns the new pointer on success, or NULL on failure (original pointer is not changed).
 */
static void* config_realloc_array(void* ptr, size_t old_count, size_t element_size) {
    size_t new_size = (old_count + 1) * element_size;
    void*  new_ptr = realloc(ptr, new_size);
    return new_ptr;
}

config_directive* config_directive_create(const char* name) {
    config_directive* directive = (config_directive*)malloc(sizeof(config_directive));
    if (!directive) {
        return NULL;
    }

    directive->name = config_strdup(name);
    if (name && !directive->name) {
        free(directive);
        return NULL;
    }
    directive->args = NULL;
    directive->arg_count = 0;
    return directive;
}

void config_directive_free(config_directive* directive) {
    if (!directive) {
        return;
    }

    free(directive->name);

    if (directive->args) {
        for (size_t i = 0; i < directive->arg_count; i++) {
            free(directive->args[i]);
        }
        free(directive->args);
    }

    free(directive);
}

int config_directive_add_arg(config_directive* directive, const char* arg) {
    if (!directive) {
        return -1;
    }

    char* arg_copy = config_strdup(arg);
    if (arg && !arg_copy) {
        return -1; /* Allocation failed */
    }

    char** new_args =
        (char**)config_realloc_array(directive->args, directive->arg_count, sizeof(char*));
    if (!new_args && directive->arg_count > 0) {
        free(arg_copy);
        return -1;
    }

    directive->args = new_args;
    directive->args[directive->arg_count] = arg_copy;
    directive->arg_count++;
    return 0;
}

const char* config_directive_get_name(const config_directive* directive) {
    if (!directive) {
        return NULL;
    }
    return directive->name;
}

size_t config_directive_get_arg_count(const config_directive* directive) {
    if (!directive) {
        return 0;
    }
    return directive->arg_count;
}

const char* config_directive_get_arg(const config_directive* directive, size_t index) {
    if (!directive || index >= directive->arg_count) {
        return NULL;
    }
    return directive->args[index];
}

config_directive* config_directive_copy(const config_directive* directive) {
    if (!directive) {
        return NULL;
    }

    config_directive* copy = config_directive_create(directive->name);
    if (!copy) {
        return NULL;
    }

    for (size_t i = 0; i < directive->arg_count; i++) {
        if (config_directive_add_arg(copy, config_directive_get_arg(directive, i)) != 0) {
            config_directive_free(copy);
            return NULL;
        }
    }

    return copy;
}

config_block* config_block_create(const char* name) {
    config_block* block = (config_block*)malloc(sizeof(config_block));
    if (!block) {
        return NULL;
    }

    block->name = config_strdup(name);
    if (name && !block->name) {
        free(block);
        return NULL;
    }
    block->args = NULL;
    block->arg_count = 0;
    block->directives = NULL;
    block->directive_count = 0;
    block->subblocks = NULL;
    block->subblock_count = 0;
    return block;
}

void config_block_free(config_block* block) {
    if (!block) {
        return;
    }

    free(block->name);

    if (block->args) {
        for (size_t i = 0; i < block->arg_count; i++) {
            free(block->args[i]);
        }
        free(block->args);
    }

    if (block->directives) {
        for (size_t i = 0; i < block->directive_count; i++) {
            config_directive_free(block->directives[i]);
        }
        free(block->directives);
    }

    if (block->subblocks) {
        for (size_t i = 0; i < block->subblock_count; i++) {
            config_block_free(block->subblocks[i]);
        }
        free(block->subblocks);
    }

    free(block);
}

int config_block_add_arg(config_block* block, const char* arg) {
    if (!block) {
        return -1;
    }

    char* arg_copy = config_strdup(arg);
    if (arg && !arg_copy) {
        return -1;
    }

    char** new_args = (char**)config_realloc_array(block->args, block->arg_count, sizeof(char*));
    if (!new_args && block->arg_count > 0) {
        free(arg_copy);
        return -1;
    }

    block->args = new_args;
    block->args[block->arg_count] = arg_copy;
    block->arg_count++;
    return 0;
}

int config_block_add_directive(config_block* block, config_directive* directive) {
    if (!block || !directive) {
        return -1;
    }

    config_directive** new_directives = (config_directive**)config_realloc_array(
        block->directives, block->directive_count, sizeof(config_directive*));
    if (!new_directives && block->directive_count > 0) {
        return -1;
    }

    block->directives = new_directives;
    block->directives[block->directive_count] = directive;
    block->directive_count++;
    return 0;
}

int config_block_add_subblock(config_block* block, config_block* subblock) {
    if (!block || !subblock) {
        return -1;
    }

    config_block** new_subblocks = (config_block**)config_realloc_array(
        block->subblocks, block->subblock_count, sizeof(config_block*));
    if (!new_subblocks && block->subblock_count > 0) {
        return -1;
    }

    block->subblocks = new_subblocks;
    block->subblocks[block->subblock_count] = subblock;
    block->subblock_count++;
    return 0;
}

const char* config_block_get_name(const config_block* block) {
    if (!block) {
        return NULL;
    }
    return block->name;
}

size_t config_block_get_arg_count(const config_block* block) {
    if (!block) {
        return 0;
    }
    return block->arg_count;
}

const char* config_block_get_arg(const config_block* block, size_t index) {
    if (!block || index >= block->arg_count) {
        return NULL;
    }
    return block->args[index];
}

size_t config_block_get_directive_count(const config_block* block) {
    if (!block) {
        return 0;
    }
    return block->directive_count;
}

config_directive* config_block_get_directive(const config_block* block, size_t index) {
    if (!block || index >= block->directive_count) {
        return NULL;
    }
    return block->directives[index];
}

size_t config_block_get_subblock_count(const config_block* block) {
    if (!block) {
        return 0;
    }
    return block->subblock_count;
}

config_block* config_block_get_subblock(const config_block* block, size_t index) {
    if (!block || index >= block->subblock_count) {
        return NULL;
    }
    return block->subblocks[index];
}

config_block* config_block_copy(const config_block* block) {
    if (!block) {
        return NULL;
    }

    config_block* copy = config_block_create(block->name);
    if (!copy) {
        return NULL;
    }

    for (size_t i = 0; i < block->arg_count; i++) {
        if (config_block_add_arg(copy, config_block_get_arg(block, i)) != 0) {
            config_block_free(copy);
            return NULL;
        }
    }

    for (size_t i = 0; i < block->directive_count; i++) {
        config_directive* dir = config_block_get_directive(block, i);
        config_directive* dir_copy = config_directive_copy(dir);
        if (!dir_copy) {
            config_block_free(copy);
            return NULL;
        }

        if (config_block_add_directive(copy, dir_copy) != 0) {
            config_directive_free(dir_copy);
            config_block_free(copy);
            return NULL;
        }
    }

    /* recursively copy */
    for (size_t i = 0; i < block->subblock_count; i++) {
        config_block* sub_block = config_block_get_subblock(block, i);
        config_block* sub_block_copy = config_block_copy(sub_block);
        if (!sub_block_copy) {
            config_block_free(copy);
            return NULL;
        }

        if (config_block_add_subblock(copy, sub_block_copy) != 0) {
            config_block_free(sub_block_copy);
            config_block_free(copy);
            return NULL;
        }
    }

    return copy;
}
