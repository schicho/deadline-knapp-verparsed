#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <stddef.h>

/*
config directives are semicolone terminated lines like:

user             nobody;
error_log        logs/error.log notice;
worker_processes 1;
include conf.d/http;
include conf.d/stream;
include conf.d/exchange-enhanced;
*/
typedef struct config_directive config_directive;

/*
config blocks, also correctly named contexts, are something like this:

events {
    #configuration of connection processing
}

http {
    #Configuration specific to HTTP and affecting all virtual servers

    server {
        #configuration of HTTP virtual server 1
        location /one {
            #configuration for processing URIs starting with '/one'
        }
        location /two {
            #configuration for processing URIs starting with '/two'
        }
    }

    server {
        #configuration of HTTP virtual server 2
    }
}
*/
typedef struct config_block config_block;

config_directive* config_directive_create(const char* name);
void              config_directive_free(config_directive* directive);
int               config_directive_add_arg(config_directive* directive, const char* arg);
const char*       config_directive_get_name(const config_directive* directive);
size_t            config_directive_get_arg_count(const config_directive* directive);
const char*       config_directive_get_arg(const config_directive* directive, size_t index);

config_block*     config_block_create(const char* name);
void              config_block_free(config_block* block);
int               config_block_add_arg(config_block* block, const char* arg);
int               config_block_add_directive(config_block* block, config_directive* directive);
int               config_block_add_subblock(config_block* block, config_block* subblock);
const char*       config_block_get_name(const config_block* block);
size_t            config_block_get_arg_count(const config_block* block);
const char*       config_block_get_arg(const config_block* block, size_t index);
size_t            config_block_get_directive_count(const config_block* block);
config_directive* config_block_get_directive(config_block* block, size_t index);
size_t            config_block_get_subblock_count(const config_block* block);
config_block*     config_block_get_subblock(config_block* block, size_t index);

#endif /* CONFIGFILE_H */
