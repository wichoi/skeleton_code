#include <string.h>
#include <unistd.h>

#include "common.h"
#include "log.h"
#include "server-proc.h"

static char arg_parser(int argc, char **argv, char *log_level)
{
    int ret_val = RET_ERROR;

    typedef struct _log_level
    {
        char level;
        const char *str;
    } log_table_t;

    static const log_table_t log_tbl[] =
    {
        { LOG_NONE,     "-N" },
        { LOG_VERBOSE,  "-V" },
        { LOG_DEBUG,    "-D" },
        { LOG_INFO,     "-I" },
        { LOG_WARN,     "-W" },
        { LOG_ERROR,    "-E" },
        { LOG_FATAL,    "-F" },
    };

    if(argc < 2)
    {
        return ret_val;
    }

    char *level = (char*)argv[1];
    int i = 0;
    for(i = 0; i<sizeof(log_tbl)/sizeof(log_tbl[0]); i++)
    {
        if(strcmp(log_tbl[i].str, level) == 0)
        {
            *log_level = log_tbl[i].level;
            ret_val = RET_OK;
            break;
        }
    }

    return ret_val;
}

int main(int argc, char** argv)
{
    int ret_val = RET_OK;
    char log_level = LOG_INFO;

    // Ignore signal
    signal(SIGTTIN, SIG_IGN); // [1]+ Stopped (tty input) nap-client

    arg_parser(argc, argv, &log_level);
    debug_init(LOG_ENABLE, log_level, NULL);
    log_print(log_level, "Sigfox NAP server start !!!\n");

    server_proc server;
    server.init();

    while(1)
    {
        if(server.proc() != 0)
        {
            break;
        }
        usleep(10 * 1000); // 10msec
    }

    log_print(log_level, "Sigfox NAP server exit !!!\n");
    return ret_val;
}

