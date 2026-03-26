#ifndef LOGGER_H
#define LOGGER_H

typedef int LOG_LEVEL;

#define LOG_INFO 0
#define LOG_DEBUG 1
#define LOG_WARNING 2
#define LOG_ERROR 3

void cfg_log(LOG_LEVEL level, const char* format, ...);
void cfg_log_info(const char* format, ...);
void cfg_log_debug(const char* format, ...);
void cfg_log_warning(const char* format, ...);
void cfg_log_error(const char* format, ...);

#endif /* LOGGER_H */