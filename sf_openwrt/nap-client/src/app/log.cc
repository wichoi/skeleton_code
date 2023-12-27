#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "log.h"

#define TIME_STAMP_ONLY

typedef struct _log_level
{
    const char level;
    const char *str;
} log_table_t;

const static log_table_t log_tbl[] = 
{
    { LOG_NONE,     "N" },
    { LOG_VERBOSE,  "V" },
    { LOG_DEBUG,    "D" },
    { LOG_INFO,     "I" },
    { LOG_WARN,     "W" },
    { LOG_ERROR,    "E" },
    { LOG_FATAL,    "F" },
};

static FILE*        g_dbg_console = NULL;
static FILE*        g_dbg_file = NULL;
static log_type_e   g_log_enable = LOG_DISABLE;
static char         g_log_level = LOG_NONE;

static char         g_log_path[256] = {0,};
static char         g_log_back_path[256] = {0,};
static long         g_log_file_size = 256*1024; // 256 kbyte

void backup_log_file()
{
    if (g_dbg_file)
    {
        fclose(g_dbg_file);
        g_dbg_file = NULL;

        if (access(g_log_back_path, F_OK) == 0)
        {
            remove(g_log_back_path);
        }

        rename(g_log_path, g_log_back_path);

        g_dbg_file = fopen(g_log_path, "wt");
        if (g_dbg_file == NULL)
        {
            printf("file open error, path(%s) \n", g_log_path);
        }
    }
}

#ifndef TIME_STAMP_ONLY
static char* level_str(char level)
{
    int i = 0;
    for(i = 0; i<sizeof(log_tbl)/sizeof(log_tbl[0]); i++)
    {
        if(level == log_tbl[i].level)
        {
            return log_tbl[i].str;
        }
    }
    return NULL;
}
#endif

void debug_init(log_type_e enable, char level, const char * file_path)
{
    g_log_enable = enable;
    g_log_level = level;
    g_dbg_console = stderr; // stdout;

    if(file_path != NULL)
    {
        strcpy(g_log_path, file_path);
        strcpy(g_log_back_path, file_path);
        strcat(g_log_back_path, ".bak");

        g_dbg_file = fopen(file_path, "a+");
        if(g_dbg_file == NULL)
        {
            printf("file open error, path(%s) \n", file_path);
        }
    }
}
void debug_finish()
{
    g_dbg_console = NULL;
    if (g_dbg_file)
    {
        fclose(g_dbg_file);
        g_dbg_file = NULL;
    }
}

static char* log_prefix(int line, char level, const char * func)
{
    static char str_buf[128] = { 0x0, };
    time_t timer = time(NULL);
    struct tm *t = localtime(&timer);
    struct timeval tv;
    gettimeofday(&tv, NULL);
#ifdef TIME_STAMP_ONLY
    snprintf(str_buf, sizeof(str_buf),
        //"[%04d-%02d-%02d %02d:%02d:%02d] ",
        "[%02d:%02d:%02d:%03lu] ",
        //t->tm_year + 1900,
        //t->tm_mon + 1,
        //t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec,
        tv.tv_usec / 1000);
#else
    snprintf(str_buf, sizeof(str_buf),
        "[%04d-%02d-%02d %02d:%02d:%02d][%c][%s][%d] ",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec,
        level_str(level),
        func,
        line);
    str_buf[127] = 0;
#endif
    return str_buf;
}

void debug_print(const char *func, int line, char level, const char *format, ...)
{
    if (g_dbg_console == NULL ||
        format == NULL ||
        g_log_enable == LOG_DISABLE ||
        g_log_level == LOG_NONE ||
        g_log_level > level)
    {
        return;
    }

    const char *prefix_str = log_prefix(line, level, func);
    fprintf(g_dbg_console, "%s", prefix_str);
    if (g_dbg_file)
    {
        fprintf(g_dbg_file, "%s", prefix_str);
    }

    va_list argp;
    va_start(argp, format);
    vfprintf(g_dbg_console, format, argp);
    if (g_dbg_file)
    {
        va_start(argp, format);
        vfprintf(g_dbg_file, format, argp);
        fflush(g_dbg_file);
        if (ftell(g_dbg_file) > g_log_file_size)
        {
            backup_log_file();
        }
    }
    va_end(argp);
    //fputc('\n', g_dbg_stream);
    fflush(g_dbg_console);
}


void debug_printf(const char *format, ...)
{
    if (g_dbg_console == NULL ||
        g_log_enable == LOG_DISABLE ||
        g_log_level == LOG_NONE)
    {
        return;
    }

    va_list argp;
    va_start(argp, format);
    vfprintf(g_dbg_console, format, argp);
    if(g_dbg_file)
    {
        va_start(argp, format);
        vfprintf(g_dbg_file, format, argp);
        fflush(g_dbg_file);
        if(ftell(g_dbg_file) > g_log_file_size)
        {
            backup_log_file();
        }
    }
    va_end(argp);
}

