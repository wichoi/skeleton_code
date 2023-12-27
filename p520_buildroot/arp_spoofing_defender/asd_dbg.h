#ifndef __ARP_SPOOFING_DEFENDER_DEBUG_H__
#define __ARP_SPOOFING_DEFENDER_DEBUG_H__

typedef enum _tag_log_level
{
    LOG_NONE = 0,
    LOG_V,
    LOG_D,
    LOG_I,
    LOG_W,
    LOG_E,
    LOG_F,
    LOG_MAX
} log_level_e;

typedef enum _tag_boolean_type
{
    DBG_DISABLE = 0,
    DBG_ENABLE
} boolean_type_e;

//#ifdef DEBUG_ENABLE
#if 1
#include <stdio.h>
extern void debug_init(boolean_type_e enable, log_level_e level);
extern void debug_printf(const char *func, int line, log_level_e level, const char *format, ...);

#define log_init(enable, level)   debug_init(enable, level)
#define log_print(level, fmt,...)         debug_printf(__func__, __LINE__, level, fmt, ##__VA_ARGS__)
#else
#define debug_init(enable, level, stream)   ((void)0)
#define debug_print(...)                    ((void)0)
#endif


#endif
