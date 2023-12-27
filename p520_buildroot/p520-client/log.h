#ifndef __LOG_H__
#define __LOG_H__

#define PRINT_NONE      0
#define PRINT_VERBOSE   1
#define PRINT_DEBUG     2
#define PRINT_INFO      3
#define PRINT_WARN      4
#define PRINT_ERROR     5
#define PRINT_FATAL     6
#define PRINT_MAX       7

typedef enum _tag_log_type
{
    LOG_DISABLE = 0,
    LOG_ENABLE
} log_type_e;

void debug_init(log_type_e enable, char level, const char *file_path);
void debug_finish();

#if 1 //def DEBUG_ENABLE
void debug_print(char level, const char *format, ...);
#else
#define debug_print(...)                    ((void)0)
#endif
void debug_printf(const char *format, ...);

#define log_print(level, fmt,...) debug_print(level, fmt, ##__VA_ARGS__)

#define log_v(fmt, ...) debug_print(PRINT_VERBOSE, fmt, ##__VA_ARGS__)
#define log_d(fmt, ...) debug_print(PRINT_DEBUG  , fmt, ##__VA_ARGS__)
#define log_i(fmt, ...) debug_print(PRINT_INFO   , fmt, ##__VA_ARGS__)
#define log_w(fmt, ...) debug_print(PRINT_WARN   , fmt, ##__VA_ARGS__)
#define log_e(fmt, ...) debug_print(PRINT_ERROR  , fmt, ##__VA_ARGS__)
#define log_f(fmt, ...) debug_print(PRINT_FATAL  , fmt, ##__VA_ARGS__)

#endif
