#include "logger.h"

int main() {
    cfg_log(LOG_INFO, "test");
    cfg_log(LOG_WARNING, "this is an int %d, and a string %s", 42, "funny string");
    cfg_log_debug("aaah");
    cfg_log_error("ahhh %f", 0.123);
    cfg_log(41, "aa %s", "asd");
}