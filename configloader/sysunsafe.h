#ifndef SYSUNSAFE_H
#define SYSUNSAFE_H

extern const char* const SAFE_COMMANDS[];

/* for testing purposes. Kann wahrscheinlich weg. */
void sys_unsafe_run(const char* cmd);

int sys_sanitize_run(const char* cmd);

#endif /* SYSUNSAFE_H */