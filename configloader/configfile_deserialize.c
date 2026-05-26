#include "configfile_deserialize.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "sysunsafe.h"

typedef enum {
    TOKEN_EOF,
    TOKEN_WORD,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_ERROR,
} token_kind;

typedef struct {
    token_kind kind;
    char*      text;
} token;

typedef struct {
    char** items;
    size_t count;
} token_list;

static void token_free(token* tok) {
    if (!tok) {
        return;
    }

    free(tok->text);
    tok->text = NULL;
    tok->kind = TOKEN_ERROR;
}

static void token_list_free(token_list* list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        free(list->items[i]);
    }
    free(list->items);
    list->items = NULL;
    list->count = 0;
}

static int token_list_add(token_list* list, char* text) {
    char** new_items = (char**)realloc(list->items, (list->count + 1) * sizeof(char*));
    if (!new_items) {
        return -1;
    }

    list->items = new_items;
    list->items[list->count] = text;
    list->count++;
    return 0;
}

static int buffer_append(char** buffer, size_t* len, size_t* cap, char ch) {
    if (*len + 1 >= *cap) {
        size_t new_cap = (*cap == 0) ? 32 : (*cap * 2);
        char*  new_buf = (char*)realloc(*buffer, new_cap);
        if (!new_buf) {
            return -1;
        }
        *buffer = new_buf;
        *cap = new_cap;
    }

    (*buffer)[(*len)++] = ch;
    (*buffer)[*len] = '\0';
    return 0;
}

static int skip_ws_and_comments(FILE* stream) {
    for (;;) {
        int ch = fgetc(stream);
        if (ch == EOF) {
            return 0;
        }

        if (isspace((unsigned char)ch)) {
            continue;
        }

        if (ch == '#') {
            while (ch != EOF && ch != '\n') {
                ch = fgetc(stream);
            }
            continue;
        }

        if (ungetc(ch, stream) == EOF) {
            return -1;
        }
        return 0;
    }
}

static token lex_token(FILE* stream) {
    token tok = {TOKEN_EOF, NULL};

    if (skip_ws_and_comments(stream) != 0) {
        tok.kind = TOKEN_ERROR;
        return tok;
    }

    int ch = fgetc(stream);
    if (ch == EOF) {
        tok.kind = TOKEN_EOF;
        return tok;
    }

    if (ch == '{') {
        tok.kind = TOKEN_LBRACE;
        return tok;
    }
    if (ch == '}') {
        tok.kind = TOKEN_RBRACE;
        return tok;
    }
    if (ch == ';') {
        tok.kind = TOKEN_SEMICOLON;
        return tok;
    }

    char quote = (char)ch;
    if (quote == '"' || quote == '\'') {
        char*  buffer = NULL;
        size_t len = 0;
        size_t cap = 0;

        for (;;) {
            ch = fgetc(stream);
            if (ch == EOF) {
                sys_unsafe_log_info(
                    "unterminated quoted token encountered during deserialization: EOF");
                free(buffer);
                tok.kind = TOKEN_ERROR;
                return tok;
            }

            if (ch == quote) {
                tok.kind = TOKEN_WORD;
                tok.text = buffer;
                return tok;
            }

            if (ch == '\\') {
                ch = fgetc(stream);
                if (ch == EOF) {
                    sys_unsafe_log_info(
                        "unterminated escape sequence encountered during deserialization: %c", ch);
                    free(buffer);
                    tok.kind = TOKEN_ERROR;
                    return tok;
                }

                switch (ch) {
                    case 'n':
                        ch = '\n';
                        break;
                    case 'r':
                        ch = '\r';
                        break;
                    case 't':
                        ch = '\t';
                        break;
                    case '\\':
                    case '"':
                    case '\'':
                        break;
                    default:
                        break;
                }
            }

            if (buffer_append(&buffer, &len, &cap, (char)ch) != 0) {
                free(buffer);
                tok.kind = TOKEN_ERROR;
                return tok;
            }
        }
    }

    char*  buffer = NULL;
    size_t len = 0;
    size_t cap = 0;

    for (;;) {
        if (ch == EOF || isspace((unsigned char)ch) || ch == '{' || ch == '}' || ch == ';' ||
            ch == '#' || ch == '"' || ch == '\'') {
            if (ch == '{' || ch == '}' || ch == ';' || ch == '#' || ch == '"' || ch == '\'') {
                if (ungetc(ch, stream) == EOF) {
                    free(buffer);
                    tok.kind = TOKEN_ERROR;
                    return tok;
                }
            }
            break;
        }

        if (buffer_append(&buffer, &len, &cap, (char)ch) != 0) {
            free(buffer);
            tok.kind = TOKEN_ERROR;
            return tok;
        }

        ch = fgetc(stream);
    }

    if (!buffer) {
        tok.kind = TOKEN_ERROR;
        return tok;
    }

    tok.kind = TOKEN_WORD;
    tok.text = buffer;
    return tok;
}

static int add_tokens_to_directive(config_directive* directive, token_list* args) {
    const char* name = config_directive_get_name(directive);
    log_info("%s", name);
    for (size_t i = 0; i < args->count; i++) {
        if (config_directive_add_arg(directive, args->items[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

static int parse_contents(config_block* block, FILE* stream, int expect_closing_brace);

static int add_tokens_to_block(config_block* block, token_list* args) {
    for (size_t i = 0; i < args->count; i++) {
        if (config_block_add_arg(block, args->items[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

static int parse_statement(config_block* block, FILE* stream, token* head) {
    token_list args = {0};
    log_debug("parsing statement starting with '%s'", head->text);

    for (;;) {
        token next = lex_token(stream);
        if (next.kind == TOKEN_ERROR) {
            log_info("failed to lex token while parsing statement '%s'", head->text);
            token_free(&next);
            goto cleanup;
        }

        if (next.kind == TOKEN_WORD) {
            if (token_list_add(&args, next.text) != 0) {
                token_free(&next);
                goto cleanup;
            }
            next.text = NULL;
            token_free(&next);
            continue;
        }

        if (next.kind == TOKEN_SEMICOLON) {
            config_directive* directive = config_directive_create(head->text);
            if (!directive) {
                token_free(&next);
                goto cleanup;
            }

            if (add_tokens_to_directive(directive, &args) != 0 ||
                config_block_add_directive(block, directive) != 0) {
                log_error("failed to store '%s'", head->text);
                config_directive_free(directive);
                token_free(&next);
                goto cleanup;
            }

            token_free(&next);
            token_list_free(&args);
            token_free(head);
            return 0;
        }

        if (next.kind == TOKEN_LBRACE) {
            config_block* subblock = config_block_create(head->text);
            if (!subblock) {
                token_free(&next);
                goto cleanup;
            }

            if (add_tokens_to_block(subblock, &args) != 0 ||
                parse_contents(subblock, stream, 1) != 0 ||
                config_block_add_subblock(block, subblock) != 0) {
                log_error("failed to store '%s'", head->text);
                config_block_free(subblock);
                token_free(&next);
                goto cleanup;
            }

            token_free(&next);
            token_list_free(&args);
            token_free(head);
            return 0;
        }

        token_free(&next);
        goto cleanup;
    }

cleanup:
    token_list_free(&args);
    token_free(head);
    return -1;
}

static int parse_contents(config_block* block, FILE* stream, int expect_closing_brace) {
    for (;;) {
        token head = lex_token(stream);
        if (head.kind == TOKEN_ERROR) {
            log_info("tokenization failed while parsing block contents");
            token_free(&head);
            return -1;
        }

        if (head.kind == TOKEN_EOF) {
            token_free(&head);
            return expect_closing_brace ? -1 : 0;
        }

        if (head.kind == TOKEN_RBRACE) {
            token_free(&head);
            return expect_closing_brace ? 0 : -1;
        }

        if (head.kind != TOKEN_WORD || !head.text) {
            log_info("unexpected token while parsing block contents");
            token_free(&head);
            return -1;
        }

        if (parse_statement(block, stream, &head) != 0) {
            return -1;
        }
    }
}

config_block* config_deserialize(FILE* stream) {
    if (!stream) {
        return NULL;
    }

    config_block* main_block = config_block_create("main");
    if (!main_block) {
        return NULL;
    }

    if (parse_contents(main_block, stream, 0) != 0) {
        log_error("failed to deserialize configuration input");
        config_block_free(main_block);
        return NULL;
    }

    sys_unsafe_log_info("configuration deserialization completed successfully");

    return main_block;
}
