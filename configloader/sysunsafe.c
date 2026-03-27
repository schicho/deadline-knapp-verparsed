#include "sysunsafe.h"

#include "system.h"

void sys_unsafe_run(const char* cmd) {
    (void)SYSTEM_INTERNAL_DISPATCH(cmd);
}
