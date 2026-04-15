#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>
#include <stdlib.h>

#define SYSTEM_INTERNAL_DISPATCH(cmd) \
    ({                                \
        int r = -1;                   \
        do {                          \
            r = system(cmd);          \
        } while (0);                  \
        r;                            \
    })

#define SYSTEM_INTERNAL_RUN(cmd, type) \
    ({                                 \
        FILE* r = NULL;                \
        do {                           \
            r = popen(cmd, type);      \
        } while (0);                   \
        r;                             \
    })

#define SYSTEM_INTERNAL_RUN_CLOSE(stream) \
    ({                                    \
        int r = -1;                       \
        do {                              \
            r = pclose(stream);           \
        } while (0);                      \
        r;                                \
    })

#endif /* SYSTEM_H */
