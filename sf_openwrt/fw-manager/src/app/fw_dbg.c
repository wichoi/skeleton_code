#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fw_dbg.h"

static FILE*           g_dbg_console = NULL;
static boolean_type_e  g_dbg_enable = DBG_DISABLE;
static log_level_e     g_dbg_log_level = (log_level_e)LOG_NONE;

static char log_char(log_level_e level)
{
    char ret_val = 'N';
    switch (level)
    {
    case LOG_V:
        ret_val = 'V';
        break;

    case LOG_D:
        ret_val = 'D';
        break;

    case LOG_I:
        ret_val = 'I';
        break;

    case LOG_W:
        ret_val = 'W';
        break;

    case LOG_E:
        ret_val = 'E';
        break;

    default:
        ret_val = 'N';
        break;
    }

    return ret_val;
}

static char* time_to_str(struct tm *t, int line, log_level_e level, const char * func)
{
    static char str_buf[128] = { 0x0, };

    snprintf(str_buf, 128,
        "[%02d%02d %02d:%02d:%02d][%c][%s:%d] ",
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec,
        log_char(level),
        func, line);

    str_buf[127] = 0;

    return str_buf;
}

void debug_init(boolean_type_e enable, log_level_e level)
{
    g_dbg_enable = enable;
    g_dbg_log_level = level;
    g_dbg_console = stdout; // stderr;
}

void debug_printf(const char *func, int line, log_level_e level, const char *format, ...)
{
    if (format == NULL ||
        g_dbg_enable == DBG_DISABLE ||
        g_dbg_log_level == LOG_NONE ||
        g_dbg_log_level > level)
    {
        return;
    }

    if(g_dbg_console == NULL)
    {
        return;
    }

    time_t timer = time(NULL);
    struct tm *t = localtime(&timer);
    const char * prefix_time = time_to_str(t, line, level, func);

    fprintf(g_dbg_console, "%s", prefix_time);
    va_list argp;
    va_start(argp, format);
    vfprintf(g_dbg_console, format, argp);
    va_end(argp);

    fflush(g_dbg_console);
}

