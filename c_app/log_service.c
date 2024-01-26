#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "log_service.h"

typedef struct _log_level
{
    const char level;
    const char *str;
} log_table_t;

const static log_table_t log_tbl[] =
{
    { PRINT_NONE,       "N" },
    { PRINT_VERBOSE,    "V" },
    { PRINT_DEBUG,      "D" },
    { PRINT_INFO,       "I" },
    { PRINT_WARN,       "W" },
    { PRINT_ERROR,      "E" },
    { PRINT_FATAL,      "F" },
};

static FILE*        g_dbg_console = NULL;
static FILE*        g_dbg_file = NULL;
static log_type_e   g_log_enable = LOG_DISABLE;
static char         g_log_level = PRINT_NONE;

static char         g_log_path[256] = {0,};
static char         g_log_back_path[256] = {0,};
static long         g_log_file_size = 256*1024; // 256 kbyte

void backup_log_file()
{
    if(g_dbg_file)
    {
        fclose(g_dbg_file);
        g_dbg_file = NULL;

        if (access(g_log_back_path, F_OK) == 0)
        {
            remove(g_log_back_path);
        }

        rename(g_log_path, g_log_back_path);

        g_dbg_file = fopen(g_log_path, "wt");
        if(g_dbg_file == NULL)
        {
            printf("file open error, path(%s) \n", g_log_path);
        }
    }
}

void debug_init(log_type_e enable, char level, const char *file_path)
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
    if(g_dbg_file)
    {
        fclose(g_dbg_file);
        g_dbg_file = NULL;
    }
}

static char* log_prefix(void)
{
    static char str_buf[128] = {0,};
    time_t timer = time(NULL);
    struct tm *t = localtime(&timer);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    snprintf(str_buf, sizeof(str_buf),
        "[%02d:%02d:%02d.%03ld] ",
        t->tm_hour,
        t->tm_min,
        t->tm_sec,
        tv.tv_usec / 1000);
    return str_buf;
}

void debug_print(char level, const char *format, ...)
{
    if(g_dbg_console == NULL ||
       format == NULL ||
       g_log_enable == LOG_DISABLE ||
       g_log_level == PRINT_NONE ||
       g_log_level > level)
    {
        return;
    }

    const char *prefix_str = log_prefix();
    fprintf(g_dbg_console, "%s", prefix_str);
    if(g_dbg_file)
    {
        fprintf(g_dbg_file, "%s", prefix_str);
    }

    va_list argp;
    va_start(argp, format);
    vfprintf(g_dbg_console, format, argp);
    if(g_dbg_file)
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
    if(g_dbg_console == NULL ||
       g_log_enable == LOG_DISABLE ||
       g_log_level == PRINT_NONE)
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

