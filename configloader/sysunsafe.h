#ifndef SYSUNSAFE_H
#define SYSUNSAFE_H

const char const* SAFE_COMMANDS[] = {
    "hostname", "uname -a", "date", "whoami", "true", "false", "pwd",
};

/* for testing purposes. Kann wahrscheinlich weg. */
void sys_unsafe_run(const char* cmd);

int sys_sanitize_run(const char* cmd);

#endif /* SYSUNSAFE_H */