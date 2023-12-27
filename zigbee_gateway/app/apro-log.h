#ifndef __APRO_LOG_H__
#define __APRO_LOG_H__

#define LOG_NONE    0
#define LOG_VERBOSE 1
#define LOG_DEBUG   2
#define LOG_INFO    3
#define LOG_WARN    4
#define LOG_ERROR   5
#define LOG_FATAL   6
#define LOG_MAX     7

typedef enum _tag_log_type
{
    LOG_DISABLE = 0,
    LOG_ENABLE
} log_type_e;

void debug_init(log_type_e enable, char level, const char * file_path);
void debug_finish();

#if 1 //def DEBUG_ENABLE
void debug_print(const char *func, int line, char level, const char *format, ...);
#else
#define debug_print(...)                    ((void)0)
#endif
void debug_printf(const char *format, ...);

#define log_print(level, fmt,...) debug_print(__func__, __LINE__, level, fmt, ##__VA_ARGS__)

#define log_v(fmt, ...) debug_print(__func__, __LINE__, LOG_VERBOSE, fmt, ##__VA_ARGS__)
#define log_d(fmt, ...) debug_print(__func__, __LINE__, LOG_DEBUG  , fmt, ##__VA_ARGS__)
#define log_i(fmt, ...) debug_print(__func__, __LINE__, LOG_INFO   , fmt, ##__VA_ARGS__)
#define log_w(fmt, ...) debug_print(__func__, __LINE__, LOG_WARN   , fmt, ##__VA_ARGS__)
#define log_e(fmt, ...) debug_print(__func__, __LINE__, LOG_ERROR  , fmt, ##__VA_ARGS__)
#define log_f(fmt, ...) debug_print(__func__, __LINE__, LOG_FATAL  , fmt, ##__VA_ARGS__)

#endif
