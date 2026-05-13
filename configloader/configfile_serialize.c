#include "configfile_serialize.h"

#include <stddef.h>

#include "printer.h"
#include "string.h"

#define INDENT_BY "    "

static inline int indent(int indent_level, FILE* stream) {
    for (int i = 0; i < indent_level; i++) {
        cfg_fprintf(stream, INDENT_BY);
    }
    return 0;
}

static int serialize_directive(config_directive* directive, int indent_level, FILE* stream) {
    indent(indent_level, stream);
    size_t arg_count = config_directive_get_arg_count(directive);
    // short directive like internal;
    if (arg_count == 0) {
        cfg_fprintf(stream, "%s;\n", config_directive_get_name(directive));
        return 0;
    }

    cfg_fprintf(stream, "%s ", config_directive_get_name(directive));
    // all but the last one
    for (size_t i = 0; i < arg_count - 1; i++) {
        cfg_fprintf(stream, "%s ", config_directive_get_arg(directive, i));
    }
    // last one terminated with a semicolon.
    cfg_fprintf(stream, "%s;\n", config_directive_get_arg(directive, arg_count - 1));
    return 0;
}

static int serialize_block(config_block* block, int indent_level, FILE* stream) {
    const char* block_name = config_block_get_name(block);
    int         is_not_main = strncmp(block_name, "main", sizeof("main"));
    int         in_context_indent = is_not_main ? indent_level + 1 : indent_level;

    /* open context */
    if (is_not_main) {
        indent(indent_level, stream);
        size_t arg_count = config_block_get_arg_count(block);
        cfg_fprintf(stream, "%s ", block_name);
        for (size_t i = 0; i < arg_count; i++) {
            cfg_fprintf(stream, "%s ", config_block_get_arg(block, i));
        }
        cfg_fprintf(stream, "{\n");
    }

    size_t directive_count = config_block_get_directive_count(block);
    for (size_t i = 0; i < directive_count; i++) {
        config_directive* directive = config_block_get_directive(block, i);
        serialize_directive(directive, in_context_indent, stream);
    }
    /* some white space before subblocks */
    cfg_fprintf(stream, "\n");

    /* recursively serialize sub blocks */
    size_t subblock_count = config_block_get_subblock_count(block);
    for (size_t i = 0; i < subblock_count; i++) {
        config_block* subblock = config_block_get_subblock(block, i);
        serialize_block(subblock, in_context_indent, stream);
    }

    /* close context */
    if (is_not_main) {
        indent(indent_level, stream);
        cfg_fprintf(stream, "}\n");
    }
    return 0;
}

int config_serialize(config_block* block, FILE* stream) {
    return serialize_block(block, 0, stream);
}
