#ifndef SYSUNSAFE_H
#define SYSUNSAFE_H

extern const char* const SYS_SAFE_COMMANDS[];

/* for testing purposes. Kann wahrscheinlich weg. */
int sys_unsafe_run(const char* cmd);

int sys_unsafe_sanitize_run(const char* cmd);

int sys_unsafe_whitelist_run(const char* cmd);

#endif /* SYSUNSAFE_H */