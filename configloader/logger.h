#ifndef LOGGER_H
#define LOGGER_H

typedef int LOG_LEVEL;

#define LOG_INFO    0
#define LOG_DEBUG   1
#define LOG_WARNING 2
#define LOG_ERROR   3

void log_lvl(LOG_LEVEL level, const char* format, ...);
void log_info(const char* format, ...);
void log_debug(const char* format, ...);
void log_warning(const char* format, ...);
void log_error(const char* format, ...);

#endif /* LOGGER_H */