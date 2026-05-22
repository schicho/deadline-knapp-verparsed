#ifndef SYSUNSAFE_H
#define SYSUNSAFE_H

#include "logger.h"

extern const char* const SYS_SAFE_COMMANDS[];

int sys_unsafe_run(const char* cmd);

int sys_unsafe_sanitize_run(const char* cmd);

int sys_unsafe_whitelist_run(const char* cmd);

void sys_unsafe_log_set_lvl_visibility(LOG_LEVEL level);
void sys_unsafe_log_lvl(LOG_LEVEL level, const char* format, ...);
void sys_unsafe_log_info(const char* format, ...);
void sys_unsafe_log_debug(const char* format, ...);
void sys_unsafe_log_warning(const char* format, ...);
void sys_unsafe_log_error(const char* format, ...);

#endif /* SYSUNSAFE_H */